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
	//inicializamos la lista de personajes
	ret->personajes = list_create();

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
	//lo agregamos a la lista de personajes del planificador
	list_add(self->personajes, personaje);
	//informamos al usuario
	logger_info(get_logger(self), "El personaje %s entro al nivel", nombre);
	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PLANIFICADOR);
}

private void planificador_liberar_personaje(tad_planificador* self, tad_personaje* personaje){
	socket_close(personaje->socket);
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
	void liberar_personaje(void* personaje){
		planificador_liberar_personaje(self, personaje);
	}
	list_destroy_and_destroy_elements(self->personajes, liberar_personaje);

	//liberamos los recursos de los datos del nivel
	//socket_close(self->nivel->socket); //TODO habilitar esto cuando los niveles se conecten
	dealloc(self->nivel);

	//liberamos los recursos propios del planificador
	logger_dispose_instance(self->logger);
	dealloc(self);
}





/***************************************
 * LOGICA ******************************
 ***************************************/

void planificador_ejecutar(PACKED_ARGS){
	//UNPACK_ARG(tad_planificador* self);

	//TODO logica de planificador; multiplexor; comunicacion con nivel y personajes; etc
}
