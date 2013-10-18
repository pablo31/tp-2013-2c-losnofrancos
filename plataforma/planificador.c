/*
 * planificador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/multiplexor/multiplexor.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/common.h"
#include "../libs/protocol/protocol.h"

#include "planificador.h"

/***************************************
 * GETTERS *****************************
 ***************************************/

private char* get_nombre_nivel(tad_planificador* self){
	return self->nivel->nombre;
}

private tad_logger* get_logger(tad_planificador* self){
	return self->logger;
}

char* planificador_nombre_nivel(tad_planificador* self){
	return get_nombre_nivel(self);
}



/***************************************
 * CREACION ****************************
 ***************************************/

tad_planificador* planificador_crear(char* nombre_nivel, tad_socket* socket_nivel){
	//alojamos una estructura tad_planificador
	alloc(ret, tad_planificador);
	//obtenemos una instancia del logger
	ret->logger = logger_new_instance("Planificador %s", nombre_nivel);
	//guardamos los datos del nivel
	alloc(nivel, tad_nivel);
	nivel->nombre = nombre_nivel;
	nivel->socket = socket_nivel;
	ret->nivel = nivel;
	//inicializamos las colas de personajes
	ret->personajes_listos = queue_create();
	ret->personajes_bloqueados = queue_create();
	//inicializamos el multiplexor y le bindeamos el socket del nivel
	var(m, multiplexor_create());
//	multiplexor_bind_socket(m, socket_nivel, funcion, 2, ret, nivel); //TODO habilitar
	ret->multiplexor = m;

	logger_info(get_logger(ret), "Planificador del Nivel %s inicializado", nombre_nivel);
	return ret;
}

/***************************************
 * MANEJO DE PERSONAJES ****************
 ***************************************/

void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket){
	//alojamos una instancia de tad_personaje
	alloc(personaje, tad_personaje);
	personaje->nombre = nombre;
	personaje->simbolo = simbolo;
	personaje->socket = socket;
	//asociamos su socket al multiplexor
//	multiplexor_bind_socket(self->multiplexor, socket, funcion, 2, self, personaje); //TODO habilitar
	//lo agregamos a la lista de personajes listos del planificador
	queue_push(self->personajes_listos, personaje);
	//informamos al usuario
	logger_info(get_logger(self), "El personaje %s entro al nivel", nombre);
	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PLANIFICADOR);
}

private void planificador_liberar_personaje(tad_planificador* self, tad_personaje* personaje){
	var(socket, personaje->socket);
	multiplexor_unbind_socket(self->multiplexor, socket);
	socket_close(socket);
	var(nombre, personaje->nombre);
	logger_info(get_logger(self), "El personaje %s fue pateado", nombre);
	free(nombre);
	dealloc(personaje);
}

/***************************************
 * FINALIZACION ************************
 ***************************************/

void planificador_finalizar(tad_planificador* self){
	logger_info(get_logger(self), "Finalizando");

	//liberamos los recursos de los datos de los personajes
	void destroyer(void* ptr_personaje){
		planificador_liberar_personaje(self, ptr_personaje);
	}
	queue_destroy_and_destroy_elements(self->personajes_listos, destroyer);
	queue_destroy_and_destroy_elements(self->personajes_bloqueados, destroyer);

	//liberamos los recursos de los datos del nivel
	var(m, self->multiplexor);
	var(nivel, self->nivel);
//	var(socket, nivel->socket);
//	multiplexor_unbind_socket(m, socket);
//	socket_close(socket); //TODO habilitar esto cuando los niveles se conecten
	dealloc(nivel);

	//liberamos los recursos del multiplexor
	multiplexor_dispose(m);

	//liberamos los recursos propios del planificador
	logger_dispose_instance(self->logger);
	dealloc(self);
}





/***************************************
 * LOGICA ******************************
 ***************************************/

//private void paquete_entrante_nivel(tad_planificador* self){
//	var(socket, self->nivel->socket);
//	SOCKET_ON_ERROR(socket, planificador_finalizar(self)); //TODO hacer una func manejadora de desconexioon de nivel
//
//}
//private void manejadora_nivel(PACKED_ARGS){
//	UNPACK_ARG(tad_planificador* self);
//	paquete_entrante_nivel(self);
//}















void planificador_ejecutar(PACKED_ARGS){
//	UNPACK_ARG(tad_planificador* self);
//
//	var(listos, self->personajes_listos);
//	var(bloqueados, self->personajes_bloqueados);
//
//	int quantum = 2; //TODO levantar desde archivo
//	int i;

	//TODO logica de planificador; multiplexor; comunicacion con nivel y personajes; etc
}



