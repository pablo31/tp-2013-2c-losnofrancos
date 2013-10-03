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

#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/protocol/protocol.h"
#include "../libs/common.h"

#include "personaje.h"



private bool verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Debe ingresar los nombres de los archivos log y configuracion.\n");
		return false;
	}
	return true;
}

//inicializacion y destruccion
private t_personaje* personaje_crear(char* config_path);
private void personaje_destruir(t_personaje* self);
//getters & setters
private tad_logger* get_logger(t_personaje* self);
private char* get_nombre(t_personaje* self);
private t_list* get_niveles(t_personaje* self);
private int get_vidas_iniciales(t_personaje* self);
private int get_vidas(t_personaje* self);
private void set_vidas(t_personaje* self, int value);
//logica y ejecucion
private void morir(t_personaje* self);
private void comer_honguito_verde(t_personaje* self);
private void jugar_nivel(PACKED_ARGS);











int main(int argc, char* argv[]) {

	if (!verificar_argumentos(argc, argv)) return EXIT_FAILURE;

	char* exe_name = argv[0];
	char* config_file = argv[1];
	char* log_file = argv[2];

	//inicializamos el singleton logger
	logger_initialize_for_debug(config_file, exe_name);

	//levantamos el archivo de configuracion
	t_personaje* self = personaje_crear(argv[1]);
	logger_debug(get_logger(self), "Personaje %s creado", get_nombre(self));

	//declaramos las funciones manejadoras de senales
	signal_dynamic_handler(SIGTERM, morir(self));
	signal_dynamic_handler(SIGUSR1, comer_honguito_verde(self));
	logger_debug(get_logger(self), "Senales establecidas");



	var(niveles, get_niveles(self));
	logger_debug(get_logger(self), "var niveles");
	var(cantidad_de_niveles, list_size(niveles));
	logger_debug(get_logger(self), "var size");
	tad_thread* thread[cantidad_de_niveles];
	logger_debug(get_logger(self), "declaracion threads");

	int i;

	//iniciamos un nuevo hilo por cada nivel que tenemosq ue jugar
	for(i = 0; i < cantidad_de_niveles; i++){
		t_nivel* nivel = list_get(niveles, i);

		thread[i] = thread_begin(jugar_nivel, 2, self, nivel);
	}

	//esperamos a que todos los hilos terminen
	for(i = 0; i < cantidad_de_niveles; i++){
		thread_join(thread[i]);
	}




	//TODO conectarse con oruqestador y decirle que cumplimos los objetivos!

	personaje_destruir(self);
	logger_dispose();
	signal_dispose_all();
	return EXIT_SUCCESS;
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



private t_personaje* personaje_crear(char* config_path){
	alloc(ret, t_personaje);
	ret->logger = logger_new_instance("");

	//TODO inicializar los otros atributos internos y quitar este hardcod

	ret->nombre = "Mario";
	ret->simbolo = '@';
	ret->vidas_iniciales = 10;
	ret->vidas = 10;
	ret->ippuerto_orquestador = "127.0.0.1:27015";

	t_list* niveles = list_create();
	int i;
	for(i = 0; i < 4; i++){
		alloc(nivel, t_nivel);
		nivel->nro_nivel = i + 1;
		list_add(niveles, nivel);
	}
	ret->niveles = niveles;

	return ret;
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

	//TODO liberar ippuerto_orquestador y nombre???
}

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






private void jugar_nivel(PACKED_ARGS){
	UNPACK_ARG(t_personaje* self);
	UNPACK_ARG(t_nivel* nivel);

	var(nro_nivel, nivel->nro_nivel);

	tad_logger* logger = logger_new_instance("Thread nivel %d", nro_nivel);
	logger_debug(logger, "Hilo para jugar el nivel %d iniciado", nro_nivel);

	var(ippuerto_orquestador, self->ippuerto_orquestador); //TODO hacer un getter
	var(ip, string_get_ip(ippuerto_orquestador));
	var(puerto, string_get_port(ippuerto_orquestador));

	tad_socket* socket = socket_connect(ip, puerto);

	DECLARE_ERROR_MANAGER{
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
		logger_dispose_instance(logger);
		return;
	}FOR_SOCKET(socket);

	byte presentacion = socket_receive_empty_package(socket);
	if(presentacion != PRESENTACION_ORQUESTADOR) socket_set_error(socket, UNEXPECTED_PACKAGE);
	logger_info(logger, "El servidor es un orquestador");

	sleep(2);

	socket_send_empty_package(socket, PRESENTACION_PERSONAJE);

	logger_info(logger, "Enviando datos del personaje");
	var(nombre, get_nombre(self));
	var(simbolo, get_simbolo(self));
	socket_send_string(socket, PERSONAJE_NOMBRE, nombre);
	socket_send_char(socket, PERSONAJE_SIMBOLO, simbolo);

	sleep(2);

	logger_info(logger, "Enviando solicitud de conexion al nivel");
	socket_send_int(socket, PERSONAJE_SOLICITUD_NIVEL, nro_nivel);


	//TODO
	sleep(2);

	socket_close(socket);
	logger_dispose_instance(logger);
}




//t_personaje* personaje_create(char* config_path) {
//	t_personaje* new = malloc(sizeof(t_personaje));
//	t_config* config = config_create(config_path); //commons... andres para nivel es igual...
//
//	new->nombre = string_duplicate(config_get_string_value(config, "nombre"));
//
//	char* s = string_duplicate(config_get_string_value(config, "simbolo"));
//	new->simbolo = s[0];
//
//	new->plan_de_niveles = config_get_array_value(config, "planDeNiveles");
//	new->objetivos = _personaje_load_objetivos(config, new->plan_de_niveles);
//	new->vidas = config_get_int_value(config, "vidas");
//	new->orquestador_info = connection_create(
//			config_get_string_value(config, "orquestador"));
//
//	void morir(char* mensaje) {
//		config_destroy(config);
//		free(s);
//		personaje_destroy(new);
//		printf("Error en el archivo de configuración: %s\n", mensaje);
//	}
//
//	if (!config_has_property(config, "puerto")) {
//		morir("Falta el puerto");
//		return NULL ;
//	}
//	new->puerto = config_get_int_value(config, "puerto");
//
//	char* log_file = "personaje.log";
//	char* log_level = "INFO";
//	if (config_has_property(config, "logFile")) {
//		log_file = string_duplicate(config_get_string_value(config, "logFile"));
//	}
//	if (config_has_property(config, "logLevel")) {
//		log_level = string_duplicate(
//				config_get_string_value(config, "logLevel"));
//	}
//	new->logger = log_create(log_file, "Personaje", true,
//			log_level_from_string(log_level));
//	config_destroy(config);
//
//	new->socket_orquestador = NULL;
//	new->nivel_actual = NULL;
//	new->posicion = NULL;
//	new->posicion_objetivo = NULL;
//	new->nivel_finalizado = false;
//	new->nivel_actual_index = 0;
//	new->vidas_iniciales = new->vidas;
//	new->objetivos_array = NULL;
//	new->objetivo_actual = NULL;
//	new->objetivo_actual_index = 0;
//	new->is_blocked = false;
//
//	free(s);
//	free(log_file);
//	free(log_level);
//	return new;
//}



//void personaje_destroy(t_personaje* self) {
//	free(self->nombre);
//	array_destroy(self->plan_de_niveles);
//	dictionary_destroy_and_destroy_elements(self->objetivos,
//			(void*) array_destroy);
//	connection_destroy(self->orquestador_info);
//	log_destroy(self->logger);
//	if (self->socket_orquestador != NULL ) {
//		if (self->socket_orquestador->serv_socket != NULL ) {
//			sockets_destroy(self->socket_orquestador->serv_socket);
//		}
//		sockets_destroyClient(self->socket_orquestador);
//	}
//	if (self->nivel_actual != NULL ) {
//		personaje_nivel_destroy(self->nivel_actual);
//	}
//	if (self->posicion != NULL ) {
//		posicion_destroy(self->posicion);
//	}
//
//	if (self->posicion_objetivo != NULL ) {
//		posicion_destroy(self->posicion_objetivo);
//	}
//
//	if (self->objetivo_actual != NULL ) {
//		free(self->objetivo_actual);
//	}
//
//	free(self);
//}


//bool personaje_conectar_a_orquestador(t_personaje* self) {
//
//	self->socket_orquestador = sockets_conectar_a_servidor(NULL, self->puerto,
//			self->orquestador_info->ip, self->orquestador_info->puerto,
//			self->logger, M_HANDSHAKE_PERSONAJE, PERSONAJE_HANDSHAKE,
//			HANDSHAKE_SUCCESS, "Orquestador");
//
//	if (self->socket_orquestador == NULL ) {
//		return false;
//	}
//
//	t_mensaje* mensaje = mensaje_recibir(self->socket_orquestador);
//
//	if (mensaje == NULL ) {
//		log_error(self->logger,
//				"Personaje %s: El orquestador se ha desconectado.",
//				self->nombre);
//		return false;
//	}
//
//	if (mensaje->type != M_GET_SYMBOL_PERSONAJE_REQUEST) {
//		mensaje_destroy(mensaje);
//		return false;
//	}
//
//	mensaje_destroy(mensaje);
//
//	char* simbolo = string_from_format("%c", self->simbolo);
//
//	mensaje_create_and_send(M_GET_SYMBOL_PERSONAJE_RESPONSE,
//			string_duplicate(simbolo), strlen(simbolo) + 1,
//			self->socket_orquestador);
//	free(simbolo);
//
//	return true;
//}


//bool personaje_conectar_a_planificador(t_personaje* self) {
//	self->nivel_actual->socket_planificador = sockets_conectar_a_servidor(NULL,
//			self->puerto, self->nivel_actual->planificador->ip,
//			self->nivel_actual->planificador->puerto, self->logger,
//			M_HANDSHAKE_PERSONAJE, PERSONAJE_HANDSHAKE, HANDSHAKE_SUCCESS,
//			"Planificador");
//
//	if (self->nivel_actual->socket_planificador == NULL ) {
//		return false;
//	}
//
//	t_mensaje* mensaje = mensaje_recibir(
//			self->nivel_actual->socket_planificador);
//
//	if (mensaje == NULL ) {
//		log_error(self->logger,
//				"Personaje %s: El planificador del nivel %s se ha desconectado.",
//				self->nombre, self->nivel_actual->nombre);
//		return false;
//	}
//
//	if (mensaje->type != M_GET_SYMBOL_PERSONAJE_REQUEST) {
//		mensaje_destroy(mensaje);
//		return false;
//	}
//
//	mensaje_destroy(mensaje);
//
//	char* simbolo = string_from_format("%c", self->simbolo);
//
//	mensaje_create_and_send(M_GET_SYMBOL_PERSONAJE_RESPONSE,
//			string_duplicate(simbolo), strlen(simbolo) + 1,
//			self->nivel_actual->socket_planificador);
//	free(simbolo);
//
//	return true;
//}
