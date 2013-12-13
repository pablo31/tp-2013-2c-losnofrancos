/*
 * personaje.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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


//inicializacion y destruccion
extern t_personaje* personaje_crear(char* config_path);
private void personaje_finalizar(t_personaje* self);
//logica y ejecucion
private void morir_por_senal(t_personaje* self, t_hilo hilos[], int cantidad_hilos);
private void morir(t_personaje* self, char* tipo_muerte, tad_logger* logger);
private void comer_honguito_verde(t_personaje* self);

private void inicio_nuevo_hilo(PACKED_ARGS);
private int conectarse_al_nivel(t_hilo* hilo);
private int jugar_nivel(t_hilo* hilo);

private int solicitar_continue(t_personaje* self);

private void manejar_error_planificador(t_hilo* hilo);
private tad_socket* conectarse_al_orquestador(t_personaje* self, tad_logger* logger);



int main(int argc, char* argv[]) {

	verificar_argumentos(argc, argv);
	char* config_file = argv[1];

	//levantamos el archivo de configuracion
	t_personaje* self = personaje_crear(config_file);

	var(logger, get_logger(self));

	logger_info(logger, "Personaje %s creado", get_nombre(self));

	var(niveles, self->niveles);
	var(cantidad_de_niveles, list_size(niveles));
	t_hilo hilos[cantidad_de_niveles];

	//declaramos las funciones manejadoras de senales
	signal_dynamic_handler(SIGINT, personaje_finalizar(self));
	signal_dynamic_handler(SIGTERM, morir_por_senal(self, hilos, cantidad_de_niveles));
	signal_dynamic_handler(SIGUSR1, comer_honguito_verde(self));
	logger_info(logger, "Senales establecidas");


	int gano_todos_los_niveles = 0;
	int cantidad_de_reiniciadas = 0;
	int i;

	while(!gano_todos_los_niveles){
		//se inicia un nuevo hilo por cada nivel que tiene jugar
		for(i = 0; i < cantidad_de_niveles; i++){
			t_nivel* nivel = list_get(niveles, i);
			hilos[i].thread = thread_begin(inicio_nuevo_hilo, 3, self, nivel, &hilos[i]);
		}

		int todos_los_hilos_terminaron_bien = 1;

		//esperamos a que todos los hilos terminen de juegar
		for(i = 0; i < cantidad_de_niveles; i++){
			thread_join(hilos[i].thread);
			if(hilos[i].bloqueado != 0) todos_los_hilos_terminaron_bien = 0;
		}

		gano_todos_los_niveles = (self->vidas > 0) && todos_los_hilos_terminaron_bien;
		if(!gano_todos_los_niveles){
			if(solicitar_continue(self)){
				//reiniciamos el plan de nieveles
				cantidad_de_reiniciadas++;
				logger_info(logger, "Se reiniciara el plan de niveles (reintento numero %d)", cantidad_de_reiniciadas);
				self->vidas = self->vidas_iniciales;
			}else{
				//finalizamos personaje
				personaje_finalizar(self);
				return EXIT_SUCCESS;
			}
		}
	}

	//nos conectamos al orquestador
	tad_socket* socket = conectarse_al_orquestador(self, logger);

	//informamos que ganamos
	logger_info(logger, "Objetivos completados");
	socket_send_empty_package(socket, PERSONAJE_OBJETIVOS_COMPLETADOS);

	socket_close(socket);

	personaje_finalizar(self);
	return EXIT_SUCCESS;
}


private int solicitar_continue(t_personaje* self){
	if(self->auto_continue) return 1;

	int flag = -1;
	do{
		printf("El personaje se quedo sin vidas. Desea reiniciar el plan de niveles? (y/n) ");
		char input = getchar();
		if(input == 'y' || input == 'Y'){
			flag = 1;
		}else if(input == 'n' || input == 'N'){
			flag = 0;
		}else{
			printf("\nEl caracter ingresado no es valido.\n");
		}
	}while(flag < 0);
	return flag;
}


private tad_socket* conectarse_al_orquestador(t_personaje* self, tad_logger* logger){
	var(ippuerto_orquestador, self->ippuerto_orquestador);
	var(ip, string_get_ip(ippuerto_orquestador));
	var(puerto, string_get_port(ippuerto_orquestador));

	logger_info(logger, "Conectando con el orquestador...");

	//conectamos con el orquestador
	tad_socket* socket = socket_connect(ip, puerto);

	//establecemos la funcion manejadora de errores y desconexion
	SOCKET_ERROR_MANAGER(socket){
		//retry
		logger_info(logger, "Error en el envio o recepcion de datos del orquestador");
		socket_close(socket);
		logger_info(logger, "Reconectando con el orquestador...");
		socket = socket_connect(ip, puerto);
	}

	//recibimos la presentacion del orquestador
	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);

	sleep(2);

	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PERSONAJE);

	//enviamos nuestros datos
	socket_send_string(socket, PERSONAJE_NOMBRE, get_nombre(self));
	socket_send_char(socket, PERSONAJE_SIMBOLO, get_simbolo(self));

	//liberamos recursos
	free(ip);
	free(puerto);

	return socket;
}

private void inicio_nuevo_hilo(PACKED_ARGS){
	UNPACK_ARGS(t_personaje* self, t_nivel* nivel, t_hilo* hilo);

	//seteamos los datos del hilo
	hilo->personaje = self;
	hilo->nivel = nivel;
	hilo->bloqueado = 0;
	hilo->logger = logger_new_instance("Thread %s", nivel->nombre);

	int status = 0;
	while(!status && self->vidas)
		status = conectarse_al_nivel(hilo);

	logger_dispose_instance(hilo->logger);
}


private void manejar_error_planificador(t_hilo* hilo){
//	if(hilo->bloqueado) return;
	var(socket, hilo->socket);
	if(socket_get_error(socket) != CUSTOM_ERROR) logger_error(hilo->logger, "Error en el envio o recepcion de datos del planificador");
	socket_close(socket);
//	hilo->bloqueado = 0;
}

private int conectarse_al_nivel(t_hilo* hilo){
	var(self, hilo->personaje);
	var(nivel, hilo->nivel);
	var(logger_nivel, hilo->logger);

	tad_socket* socket = conectarse_al_orquestador(self, logger_nivel);
	hilo->socket = socket;

	SOCKET_ERROR_MANAGER(socket){
		logger_info(logger_nivel, "Error en el envio o recepcion de datos del orquestador");
		socket_close(socket);
		return 0;
	}

	//enviamos el nombre del nivel al que nos queremos conectar
	logger_info(logger_nivel, "Enviando solicitud de derivacion al planificador");
	socket_send_string(socket, PERSONAJE_SOLICITUD_NIVEL, nivel->nombre);

	//en este punto deberiamos estar conectados al planificador
	socket_receive_expected_empty_package(socket, PRESENTACION_PLANIFICADOR);
	logger_info(logger_nivel, "Conectado con el planificador");

	//jugamos el nivel
	return jugar_nivel(hilo);
}

private tad_package* esperar_paquete_del_planificador(t_hilo* hilo, byte tipo_esperado){
	var(socket, hilo->socket);
	var(self, hilo->personaje);
	var(logger, hilo->logger);

	hilo->bloqueado = 1;

	tad_package* paquete = socket_receive_one_of_this_packages(socket, 3,
			tipo_esperado,
			MUERTE_POR_DEADLOCK,
			MUERTE_POR_ENEMIGO);

	hilo->bloqueado = 0;

	var(tipo, package_get_data_type(paquete));

	if(tipo == tipo_esperado) return paquete;

	//informamos el motivo de la muerte
	if(tipo == MUERTE_POR_DEADLOCK)  morir(self, "Muerte por deadlock", logger);
	else morir(self, "Muerte por enemigo", logger);

	//liberamos recursos
	package_dispose(paquete);

	//hacemos saltar el socket con un error fantasma
	socket_set_error(socket, CUSTOM_ERROR);

	return null;
}

private int jugar_nivel(t_hilo* hilo){
	var(socket, hilo->socket);
	var(self, hilo->personaje);
	var(nivel, hilo->nivel);
	var(logger_nivel, hilo->logger);

	SOCKET_ERROR_MANAGER(socket){
		manejar_error_planificador(hilo);
		return 0;
	}

	vector2 posicionPersonaje = vector2_new(0, 0);

	socket_send_vector2(socket, PERSONAJE_POSICION, posicionPersonaje);
	logger_info(logger_nivel, "Posicion inicial seteada en (%d,%d)", posicionPersonaje.x, posicionPersonaje.y);

	vector2 posicionDelProximoRecurso = vector2_new(-1, -1);
	vector2 posicion_de_comparacion = vector2_new(-1, -1);


	int objetivosConseguidos = 0;
	int objetivosAconseguir = list_size(nivel->objetivos);
	int i = 0;
	int eje_prox_mov = 1; //para saber en que eje tiene que hacer el proximo movimiento

	while(objetivosConseguidos < objetivosAconseguir){

		//si no tenemos mas vidas, nos desconectamos y matamos el hilo
		if(!self->vidas){
			socket_close(socket);
			logger_info(logger_nivel, "Nivel finalizado sin exito por falta de vidas");
			return 0;
		}

		//esperamos que el planificador nos otorgue un turno
		tad_package* paquete = esperar_paquete_del_planificador(hilo, PLANIFICADOR_OTORGA_TURNO);
		package_dispose(paquete);
		logger_info(logger_nivel, "Turno otorgado");

		char* ptr_objetivo = list_get(nivel->objetivos, i);
		char objetivoActual = *ptr_objetivo;

		if(vector2_equals(posicionDelProximoRecurso, posicion_de_comparacion)){
			logger_info(logger_nivel, "Solicitando ubicacion del proximo recurso");
			//se solicita la ubicacion de la caja de recursos proxima a obtener, no consume quamtum
			socket_send_char(socket, SOLICITUD_UBICACION_RECURSO, objetivoActual);

			tad_package* respuesta = esperar_paquete_del_planificador(hilo, UBICACION_RECURSO);
			posicionDelProximoRecurso = package_get_vector2(respuesta);
			package_dispose_all(respuesta);
			logger_info(logger_nivel, "La ubicacion del recurso es (%d,%d)", posicionDelProximoRecurso.x, posicionDelProximoRecurso.y);


		}else if(!vector2_equals(posicionPersonaje, posicionDelProximoRecurso)){
			//se calcula en función de su posición actual (x,y), la dirección en la que debe
			//realizar su próximo movimiento  para alcanzar la caja de recursos

			//se mueve alternadamente por los ejes
			vector2 nuevaPosicion = vector2_move_alternately(posicionPersonaje, posicionDelProximoRecurso, &eje_prox_mov);
			logger_info(logger_nivel, "Moviendose a (%d,%d)", nuevaPosicion.x, nuevaPosicion.y);
			socket_send_vector2(socket, PERSONAJE_MOVIMIENTO, nuevaPosicion);
			posicionPersonaje = nuevaPosicion;

		}else if(vector2_equals(posicionPersonaje, posicionDelProximoRecurso)){
			//se solicita una instancia del recurso en caso de estar en la posicion de la caja correspondiente
			logger_info(logger_nivel, "Solicitando instancia de recurso");
			socket_send_char(socket, PERSONAJE_SOLICITUD_RECURSO, objetivoActual);

			//se queda esperando a que le otorguen el recurso
			tad_package* respuesta = esperar_paquete_del_planificador(hilo, RECURSO_OTORGADO);
			package_dispose(respuesta);

			//recibio el recurso!
			logger_info(logger_nivel, "Recurso otorgado");
			objetivosConseguidos++;
			i++;
			posicionDelProximoRecurso = vector2_new(-1, -1);
			posicion_de_comparacion = vector2_new(-1, -1);
		}
	}

	socket_close(socket);
	logger_info(logger_nivel, "Nivel completado con exito");

	return 1;

}



private void morir_por_senal(t_personaje* self, t_hilo hilos[], int cantidad_hilos){
	int i;

	//morimos
	morir(self, "Muerte por senal", get_logger(self));

	//desbloqueamos los hilos que estaban en un recv
	for(i = 0; i < cantidad_hilos; i++)
		if(hilos[i].bloqueado){
			socket_close(hilos[i].socket);
			logger_dispose_instance(hilos[i].logger);
			pthread_kill(hilos[i].thread, SIGKILL);
		}
}

private void morir(t_personaje* self, char* tipo_muerte, tad_logger* logger){
	logger_info(logger, "El personaje muere por:  %s", tipo_muerte);

	if(self->vidas > 0){
		self->vidas--;
		if(self->vidas > 0) logger_info(logger, "El personaje perdio una vida, le quedan %d", self->vidas);
		else logger_info(logger, "El personaje perdio su ultima vida");
	}else{
		logger_info(logger, "El personaje no tiene mas vidas");
	}
}

private void comer_honguito_verde(t_personaje* self){
	self->vidas++;
	logger_info(get_logger(self), "El personaje gano una vida, ahora tiene %d", self->vidas);
}

private void personaje_finalizar(t_personaje* self){
	var(logger, get_logger(self));
	logger_info(logger, "Finalizando");

	var(niveles, self->niveles);

	foreach(nivel, niveles, t_nivel*){
		free(nivel->nombre);

		var(objetivos, nivel->objetivos);
		foreach(objetivo, objetivos, char*)
			free(objetivo);
		list_destroy(objetivos);

		dealloc(nivel);
	}
	list_destroy(niveles);

	free(self->nombre);
	free(self->ippuerto_orquestador);

	logger_dispose_instance(logger);
	dealloc(self);

	logger_dispose();
	signal_dispose_all();

	exit(EXIT_SUCCESS);
}
