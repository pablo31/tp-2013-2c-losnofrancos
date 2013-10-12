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
#include "personaje_posicion.h"
#include "personaje.h"


//porqueee..
private bool verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Debe ingresar los nombres de los archivos log y configuracion.\n");
		return false;
	}
	return true;
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

private void desconectarse_al_planificador(PACKED_ARGS);
private void conectarse_al_orquestador(PACKED_ARGS);
private void conectarse_al_planificador(PACKED_ARGS);
private void jugar_nivel(t_personaje* self, t_nivel* nivel, tad_socket* socket, tad_logger* logger);



/*Estrategia:
El personaje se conecta una ves al orquestador.
Hace el Handshake, crea los hilos planificadores correspondientes y se queda
esperando la señal "turnoConcedido"

Cada hilo recibe los mensajes de los planificadores.

Al final el hilo solo se da cuenta que el personaje termino el nivel y cierra la
conexión del socket y se destruye el hilo.
*/

int main(int argc, char* argv[]) {

	if (!verificar_argumentos(argc, argv)) return EXIT_FAILURE;

	char* exe_name = argv[0];
	char* config_file = argv[1];
	char* log_file = argv[2];

	//inicializamos el singleton logger
	logger_initialize_for_info(log_file, exe_name);

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
	tad_thread* thread[cantidad_de_niveles];


	tad_socket* socket = conectarse_al_orquestador(self);


	int i;
	//se inicia un nuevo hilo por cada nivel que tiene jugar
	for(i = 0; i < cantidad_de_niveles; i++){
		t_nivel* nivel = list_get(niveles, i);
		thread[i] = thread_begin(conectarse_al_planificador, 2, self, nivel);
	}

	personaje_destruir(self);
	logger_dispose();
	signal_dispose_all();
	return EXIT_SUCCESS;
}

private void conectarse_al_planificador(PACKED_ARGS){
	UNPACK_ARG(t_personaje* self);
	UNPACK_ARG(t_nivel* nivel);


	var(nombre_nivel, nivel->nombre);
	tad_logger* loggerPorNivel = logger_new_instance("Thread nivel %s", nivel.nombre);
	tad_socket* socket = socket_listen(self.ippuerto_orquestador);
	jugar_nivel(self, nivel,socket,loggerPorNivel);

}


private tad_socket conectarse_al_orquestador(t_personaje* self){

	var(ippuerto_orquestador, get_ippuerto_orquestador(self));
	var(ip, string_get_ip(ippuerto_orquestador));
	var(puerto, string_get_port(ippuerto_orquestador));

	tad_socket* socket = socket_connect(ip, puerto);

	DECLARE_ERROR_MANAGER{
		switch(socket_get_error(socket)){
		case CONNECTION_CLOSED:
			logger_error(self->logger, "El orquestador se desconecto inesperadamente");
			break;
		case UNEXPECTED_PACKAGE:
			logger_error(self->logger, "El orquestador envio un paquete incorrecto");
			break;
		default:
			logger_error(self->logger, "Error en el envio o recepcion de datos del orquestador");
			break;
		}
		socket_close(socket);
		logger_dispose_instance(self->logger);
		return null;
	}FOR_SOCKET(socket);

	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);
	logger_info(self->logger, "El servidor es un orquestador");

	sleep(2);

	socket_send_empty_package(socket, PRESENTACION_PERSONAJE);

	sleep(2);

	logger_info(self->logger, "Enviando datos del personaje");
	socket_send_string(socket, PERSONAJE_NOMBRE, get_nombre(self));
	socket_send_char(socket, PERSONAJE_SIMBOLO, get_simbolo(self));

	return socket;
}


private void desconectarse_al_planificador(tad_socket* socket,tad_logger* logger){
	logger_info(logger, "El personaje se desconecto del orquestador");
	socket_close(socket);
	logger_dispose_instance(logger);

}

private void jugar_nivel(t_personaje* self, t_nivel* nivel, tad_socket* socket, tad_logger* logger){

	logger_info(logger, "El personaje  %s", self->nombre);
	logger_info(logger, "Jugando al Nivel  %s", nivel->nombre);

	self->objetivo_actual_index = 0;
	self->objetivo_actual = NULL;

	//TODO espera las señales
	//esperando señales y a trabajar duro
}



private t_personaje* personaje_crear(char* config_path){
	//creamos una instancia de personaje
	alloc(self, t_personaje);
	//obtenemos una instancia del logger
	self->logger = logger_new_instance("");

	//Creamos una instancia del lector de archivos de config
	t_config* config = config_create(config_path);

	self->nombre = string_duplicate(config_get_string_value(config, "nombre"));
	self->simbolo = *config_get_string_value(config, "simbolo");

	int vidas = config_get_int_value(config, "vidas");
	self->vidas_iniciales = vidas;
	self->vidas = vidas;

	self->ippuerto_orquestador = string_duplicate(config_get_string_value(config, "orquestador"));

	//TODO levantar los niveles y objetivos del archivo de config
	t_list* niveles = list_create();
	int i;
	for(i = 0; i < 3; i++){
		alloc(nivel, t_nivel);
		nivel->nombre = string_from_format("nivel%d", i + 1);
		list_add(niveles, nivel);
	}
	self->niveles = niveles;
	self->completoTodosLosNiveles= false;
	//liberamos recursos
	config_destroy(config);

	return self;
}


t_posicion* pedir_posicion_objetivo(t_personaje* self, char* objetivo,tad_logger* logger,  tad_socket* socket) {

	log_info(logger,"Personaje:  %s",self->nombre);
	log_info(logger,"Solicitando proximo recurso:  %s",self->objetivo_actual);

	char input = self->objetivo_actual;
	tad_package* paquete = package_create('s', strlen(input) + 1, input);
	socket_send_package(socket, paquete);

	log_info(logger," Paquete enviado del Personaje:  %s",self->nombre);

	//Recibimos el paquete que estaba en espera
	tad_package* paquete = socket_receive_package(socket);

	char* texto = package_get_data(paquete);
	printf("Paquete recibido: tipo '%c', longitud %d, texto '%s'.\n",
			package_get_data_type(paquete),
			package_get_data_length(paquete),
		texto);

	//esta es la magiaaaaa WOOOo... xD
	//supuestamente el  package_get_data_type(paquete) me devuelve un par (x,y)
	t_posicion* posicion_objetivo = posicion_duplicate(package_get_data_type(paquete));

	//se libera sus recursos
	package_dispose(paquete);
	free(texto);
	return posicion_objetivo;
}



private void morir(t_personaje* self){
	var(vidas, get_vidas(self));
	var(vidas_iniciales, get_vidas_iniciales(self));

	if(vidas > 0){
		vidas--;
		logger_info(get_logger(self), "El personaje perdio una vida, le quedan %d", vidas);
		//TODO ver que nivel hay que reiniciar
	}else{
		vidas = vidas_iniciales;
		logger_info(get_logger(self), "El personaje perdio su ultima vida");
		logger_info(get_logger(self), "Las vidas se reestableceran a %d", vidas);
		//TODO ver que hacer en esta situacion jaja
	}

	set_vidas(self, vidas);
}

private void comer_honguito_verde(t_personaje* self){
	logger_info(get_logger(self), "Llego un honguito de esos que pegan ;)");
	var(vidas, get_vidas(self));
	vidas++;
	logger_info(get_logger(self), "El personaje gano una vida, posee en total %d", vidas);
	set_vidas(self, vidas);
}

private void personaje_destruir(t_personaje* self){
	var(niveles, self->niveles);
	var(size, list_size(niveles));
	int i;
	for(i = size; i > 0; i--){
		t_nivel* nivel = list_get(niveles, i);
		list_remove(niveles, i);
		dealloc(nivel);
	}
	list_destroy(niveles);

	logger_dispose_instance(get_logger(self));

	free(self->nombre);
	free(self->ippuerto_orquestador);
	free(self->posicion);
	free(self->posicion_objetivo);
	dealloc(self);
}
