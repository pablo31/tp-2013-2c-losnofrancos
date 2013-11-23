/*
 * personaje.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libs/common/string.h"
#include "../libs/common/config.h"
#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/protocol/protocol.h"
#include "../libs/common.h"
#include "personaje.h"

private void verificar_argumentos(int argc, char* argv[]) {
	if(argc > 1) return;
	printf("Error: Debe ingresar el nombre del archivo de configuracion\n");
	exit(EXIT_FAILURE);
}


/***********************************************
 * GETTERS & SETTERS ***************************
 ***********************************************/

private tad_logger* get_logger(t_personaje* self){
	return self->logger;
}

private char* get_nombre(t_personaje* self){
	return self->nombre;
}

private char get_simbolo(t_personaje* self){
	return self->simbolo;
}

private t_list* get_niveles(t_personaje* self){
	return self->niveles;
}

private int get_vidas_iniciales(t_personaje* self){
	return self->vidas_iniciales;
}

private int get_vidas(t_personaje* self){
	return self->vidas;
}

private void set_vidas(t_personaje* self, int value){
	self->vidas = value;
}

private char* get_ippuerto_orquestador(t_personaje* self){
	return self->ippuerto_orquestador;
}


//inicializacion y destruccion
private t_personaje* personaje_crear(char* config_path);
private void personaje_destruir(t_personaje* self);
//logica y ejecucion
private void morir(t_personaje* self);
private void comer_honguito_verde(t_personaje* self);

private void inicio_nuevo_hilo(PACKED_ARGS);
private int conectarse_al_orquestador(t_personaje* self, t_nivel* nivel, tad_logger* logger);
private int jugar_nivel(t_personaje* self, t_nivel* nivel, tad_socket* socket, tad_logger* logger);


private void manejar_error_orquestador(tad_socket* socket, tad_logger* logger);
private void manejar_error_planificador(tad_socket* socket, tad_logger* logger);



/*Estrategia:
El personaje se conecta una ves al orquestador.
Hace el Handshake, crea los hilos planificadores correspondientes y se queda
esperando la señal "turnoConcedido"

Cada hilo recibe los mensajes de los planificadores.

Al final el hilo solo se da cuenta que el personaje termino el nivel y cierra la
conexión del socket y se destruye el hilo.
*/

int main(int argc, char* argv[]) {

	verificar_argumentos(argc, argv);
	char* config_file = argv[1];

	//levantamos el archivo de configuracion
	t_personaje* self = personaje_crear(config_file);
	if(self == null) return EXIT_FAILURE; //TODO liberar logger

	logger_info(get_logger(self), "Personaje %s creado", get_nombre(self));

	//declaramos las funciones manejadoras de senales
	signal_dynamic_handler(SIGTERM, morir(self));
	signal_dynamic_handler(SIGUSR1, comer_honguito_verde(self));
	logger_info(get_logger(self), "Senales establecidas");


	var(niveles, get_niveles(self));
	var(cantidad_de_niveles, list_size(niveles));
	tad_thread thread[cantidad_de_niveles];

	int gano_todos_los_niveles = 0;
	int cantidad_de_reiniciadas = 0;

	while(!gano_todos_los_niveles){
		int i;
		//se inicia un nuevo hilo por cada nivel que tiene jugar
		for(i = 0; i < cantidad_de_niveles; i++){
			t_nivel* nivel = list_get(niveles, i);
			thread[i] = thread_begin(inicio_nuevo_hilo, 2, self, nivel);
		}

		//esperamos a que todos los hilos terminen de juegar
		for(i = 0; i < cantidad_de_niveles; i++){
			thread_join(thread[i]);
		}

		//si todavia tiene vidas, significa que gano todos los niveles
		gano_todos_los_niveles = get_vidas(self);
		if(!gano_todos_los_niveles){
			//TODO preguntar si reiniciar
			cantidad_de_reiniciadas++;
		}
	}

	//TODO conectarse al orquestador para decirle que ganamos

	personaje_destruir(self);
	logger_dispose();
	signal_dispose_all();
	return EXIT_SUCCESS;
}




private void inicio_nuevo_hilo(PACKED_ARGS){
	UNPACK_ARGS(t_personaje* self, t_nivel* nivel);

	//obtenemos una instancia del logger para el nivel
	tad_logger* logger_nivel = logger_new_instance("Thread %s", nivel->nombre);

	int status = 0;
	while(!status && self->vidas)
		status = conectarse_al_orquestador(self, nivel, logger_nivel);

	logger_dispose_instance(logger_nivel);
}


private void manejar_error_orquestador(tad_socket* socket, tad_logger* logger){
	switch(socket_get_error(socket)){
	case CONNECTION_CLOSED:
		logger_error(logger, "El orquestador se desconecto inesperadamente");
		break;
	case UNEXPECTED_PACKAGE:
		logger_error(logger, "El orquestador envio un paquete incorrecto");
		break;
	default:
		logger_error(logger, "Error en el envio o recepcion de datos del orquestador");
		break;
	}
	socket_close(socket);
}

private void manejar_error_planificador(tad_socket* socket, tad_logger* logger){
	if(socket_get_error(socket) != CUSTOM_ERROR) logger_error(logger, "Error en el envio o recepcion de datos del planificador");
	socket_close(socket);
}

private int conectarse_al_orquestador(t_personaje* self, t_nivel* nivel, tad_logger* logger_nivel){
	var(ippuerto_orquestador, get_ippuerto_orquestador(self));
	var(ip, string_get_ip(ippuerto_orquestador));
	var(puerto, string_get_port(ippuerto_orquestador));

	//conectamos con el orquestador
	tad_socket* socket = socket_connect(ip, puerto);
	free(ip);
	free(puerto);

	//establecemos la funcion manejadora de errores y desconexion
	SOCKET_ERROR_MANAGER(socket){
		manejar_error_orquestador(socket, logger_nivel);
		return 0;
	}

	//recibimos la presentacion del orquestador
	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);
	logger_info(logger_nivel, "El servidor es un orquestador");

	sleep(2);

	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PERSONAJE);

	//enviamos nuestros datos
	logger_info(logger_nivel, "Enviando datos del personaje");
	socket_send_string(socket, PERSONAJE_NOMBRE, get_nombre(self));
	socket_send_char(socket, PERSONAJE_SIMBOLO, get_simbolo(self));

	//enviamos el nombre del nivel al que nos queremos conectar
	logger_info(logger_nivel, "Enviando solicitud de derivacion al planificador");
	socket_send_string(socket, PERSONAJE_SOLICITUD_NIVEL, nivel->nombre);

	//en este punto deberiamos estar conectados al planificador
	socket_receive_expected_empty_package(socket, PRESENTACION_PLANIFICADOR);
	logger_info(logger_nivel, "Conectado con el planificador");

	//jugamos el nivel
	return jugar_nivel(self, nivel, socket, logger_nivel);
}

private tad_package* esperar_paquete_del_planificador(t_personaje* self, byte tipo_esperado, tad_socket* socket, tad_logger* logger){
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 3,
			tipo_esperado,
			MUERTE_POR_DEADLOCK,
			MUERTE_POR_ENEMIGO);
	var(tipo, package_get_data_type(paquete));

	if(tipo == tipo_esperado) return paquete;

	//informamos el motivo de la muerte
	if(tipo == MUERTE_POR_DEADLOCK) logger_info(logger, "Muerte por enemigo");
	else logger_info(logger, "Muerte por deadlock");

	morir(self);

	//liberamos recursos
	package_dispose(paquete);
	//hacemos saltar el socket con un error fantasma
	socket_set_error(socket, CUSTOM_ERROR);

	return null;
}

private int jugar_nivel(t_personaje* self, t_nivel* nivel, tad_socket* socket, tad_logger* logger_nivel){
	SOCKET_ERROR_MANAGER(socket){
		manejar_error_planificador(socket, logger_nivel);
		return 0;
	}

	vector2 posicionPersonaje = vector2_new();

	socket_send_vector2(socket, POSICION_INICIAL, posicionPersonaje);
	logger_info(logger_nivel, "Posicion inicial seteada en (%d,%d)", posicionPersonaje.x, posicionPersonaje.y);

	vector2 posicionDelProximoRecurso = vector2_new(-1, -1);
	vector2 posicion_de_comparacion = vector2_new(-1, -1);


	int objetivosConseguidos = 0;
	int objetivosAconseguir = list_size(nivel->objetivos);
	int i = 0;

	while(objetivosConseguidos < objetivosAconseguir){

		//si no tenemos mas vidas, nos desconectamos y matamos el hilo
		if(!get_vidas(self)){
			socket_close(socket);
			logger_info(logger_nivel, "Nivel finalizado sin exito por falta de vidas");
			return 0;
		}

		//esperamos que el planificador nos otorgue un turno
		tad_package* paquete = esperar_paquete_del_planificador(self, PLANIFICADOR_OTORGA_TURNO, socket, logger_nivel);
		package_dispose(paquete);
		logger_info(logger_nivel, "Turno otorgado");

		char* ptr_objetivo = list_get(nivel->objetivos, i);
		char objetivoActual = *ptr_objetivo;

		if(vector2_equals(posicionDelProximoRecurso, posicion_de_comparacion)){
			logger_info(logger_nivel, "Solicitando ubicacion del proximo recurso");
			//se solicita la ubicacion de la caja de recursos proxima a obtener, no consume quamtum
			socket_send_char(socket, SOLICITUD_UBICACION_RECURSO, objetivoActual);

			tad_package* respuesta = esperar_paquete_del_planificador(self, UBICACION_RECURSO, socket, logger_nivel);
			posicionDelProximoRecurso = package_get_vector2(respuesta);
			package_dispose(respuesta);
			logger_info(logger_nivel, "La ubicacion del recurso es (%d,%d)", posicionDelProximoRecurso.x, posicionDelProximoRecurso.y);


		}else if(!vector2_equals(posicionPersonaje, posicionDelProximoRecurso)){
			//se calcula en función de su posición actual (x,y), la dirección en la que debe
			//realizar su próximo movimiento  para alcanzar la caja de recursos y 1avanzar
			vector2 nuevaPosicion = vector2_next_step(posicionPersonaje, posicionDelProximoRecurso);
			logger_info(logger_nivel, "Moviendose a (%d,%d)", nuevaPosicion.x, nuevaPosicion.y);
			socket_send_vector2(socket, PERSONAJE_MOVIMIENTO, nuevaPosicion);
			posicionPersonaje = nuevaPosicion;

		}else if(vector2_equals(posicionPersonaje, posicionDelProximoRecurso)){
			//se solicita una instancia del recurso en caso de estar en la posicion de la caja correspondiente
			logger_info(logger_nivel, "Solicitando instancia de recurso");
			socket_send_char(socket, PERSONAJE_SOLICITUD_RECURSO, objetivoActual);

			//se queda esperando a que le otorguen el recurso
			tad_package* respuesta = esperar_paquete_del_planificador(self, RECURSO_OTORGADO, socket, logger_nivel);
			package_dispose(respuesta);

			//recibio el recurso!
			logger_info(logger_nivel, "Recurso otorgado");
			objetivosConseguidos++;
			i++;
			posicionDelProximoRecurso = vector2_new(-1, -1);
			posicion_de_comparacion = vector2_new(-1, -1);
		}
	}

	//informamos que ganamos y nos vamos a desconectar
	socket_send_empty_package(socket, PERSONAJE_DESCONEXION);

	socket_close(socket);
	logger_info(logger_nivel, "Nivel completado con exito");

	return 1;

}





private t_personaje* personaje_crear(char* config_path){
	//creamos una instancia de personaje
	alloc(self, t_personaje);

	//creamos una instancia del lector de archivos de config
	t_config* config = config_create(config_path);

	self->nombre = string_duplicate(config_get_string_value(config, "nombre"));
	self->simbolo = *config_get_string_value(config, "simbolo");

	//cargamos los datos del logger
	char* log_file = config_get_string_value(config, "logFile");
	char* log_level = config_get_string_value(config, "logLevel");
	logger_initialize(log_file, "personaje.sh", log_level);

	//obtenemos una instancia del logger
	self->logger = logger_new_instance();

	int vidas = config_get_int_value(config, "vidas");
	self->vidas_iniciales = vidas;
	self->vidas = vidas;

	self->ippuerto_orquestador = string_duplicate(config_get_string_value(config, "orquestador"));


	int cantidad_niveles = config_get_int_value(config, "niveles");
	char** nombres_niveles = config_get_array_value(config, "planDeNiveles");

	t_list* niveles = list_create();
	int i;
	for(i = 0; i < cantidad_niveles; i++){
		alloc(nivel, t_nivel);
		char* nombre_nivel = nombres_niveles[i];
		nivel->nombre = nombre_nivel;
		nivel->objetivos = list_create();

		char* key_cantidad_objetivos = string_from_format("objs[%s]", nombre_nivel);
		char* key_objetivos = string_from_format("obj[%s]", nombre_nivel);

		int cantidad_objetivos = config_get_int_value(config, key_cantidad_objetivos);
		char** objetivos = config_get_array_value(config, key_objetivos);

		int ii;
		for(ii = 0; ii < cantidad_objetivos; ii++){
			char* objetivo = objetivos[ii];
			list_add(nivel->objetivos, objetivo);
		}

		free(key_cantidad_objetivos);
		free(key_objetivos);

		list_add(niveles, nivel);
	}
	self->niveles = niveles;

	//liberamos recursos
	config_destroy(config);

	return self;
}




private void morir(t_personaje* self){
	var(vidas, get_vidas(self));
	var(vidas_iniciales, get_vidas_iniciales(self));

	if(vidas > 0){
		vidas--;
		logger_info(get_logger(self), "El personaje perdio una vida, le quedan %d", vidas);
	}else{
		//TODO preguntarle al usuario si quiere reestableces las vidas
		vidas = vidas_iniciales;
		logger_info(get_logger(self), "El personaje perdio su ultima vida");
		logger_info(get_logger(self), "Las vidas se reestableceran a %d", vidas);
	}

	set_vidas(self, vidas);
}

private void comer_honguito_verde(t_personaje* self){
	logger_info(get_logger(self), "Llego un honguito de esos que pegan");
	self->vidas++;
	logger_info(get_logger(self), "El personaje gano una vida, posee en total %d", get_vidas(self));
}

private void personaje_destruir(t_personaje* self){
	var(niveles, self->niveles);

	void liberar_nivel(void* ptr_nivel){
		 t_nivel* nivel = ptr_nivel;

		 var(objetivos, nivel->objetivos);
		 void liberar_objetivo(void* ptr_objetivo){
		 	char* objetivo = ptr_objetivo;
		 	free(objetivo);
		 }
		 list_destroy_and_destroy_elements(objetivos, liberar_objetivo);

		 // t_round* objetivos = nivel->objetivos;
		 // round_restart(objetivos);
		 // while(!round_has_ended(objetivos)){
			//  char* objetivo = round_remove(objetivos);
			//  free(objetivo);
		 // }
		 // round_dispose(objetivos);

		 free(nivel->nombre);
		 dealloc(nivel);
	}
	list_destroy_and_destroy_elements(niveles, liberar_nivel);

	logger_dispose_instance(get_logger(self));

	free(self->nombre);
	free(self->ippuerto_orquestador);
	dealloc(self);
}





