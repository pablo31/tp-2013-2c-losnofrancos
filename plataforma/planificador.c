/*
 * planificador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/multiplexor/multiplexor.h"
#include "../libs/common.h"

#include "planificador.h"

tad_planificador* planificador_crear(int nro_nivel, tad_socket* socket_nivel){
	//alojamos una estructura tad_planificador
	alloc(ret, tad_planificador);
	//obtenemos una instancia del logger
	ret->logger = logger_new_instance("Planificador %d", nro_nivel);
	//guardamos los datos del nivel
	alloc(nivel, tad_nivel);
	nivel->nro = nro_nivel;
	nivel->socket = socket_nivel;
	ret->nivel = nivel;
	//inicializamos la lista de personajes
	ret->personajes = list_create();

	logger_info(planificador_logger(ret), "Planificador del Nivel %d inicializado", nro_nivel);
	return ret;
}

int planificador_numero_nivel(tad_planificador* planificador){
	return planificador->nivel->nro;
}

tad_logger* planificador_logger(tad_planificador* planificador){
	return planificador->logger;
}

void planificador_ejecutar(PACKED_ARGS){
	//UNPACK_ARG(tad_planificador* planificador);

	//TODO logica del planificador..
}

void planificador_agregar_personaje(tad_planificador* planificador, char* nombre, char simbolo, tad_socket* socket){
	//alojamos una instancia de tad_personaje
	alloc(personaje, tad_personaje);
	personaje->nombre = nombre;
	personaje->simbolo = simbolo;
	personaje->socket = socket;
	//lo agregamos a la lista de personajes del planificador
	list_add(planificador->personajes, personaje);
	//informamos al usuario
	logger_info(planificador_logger(planificador), "El personaje %s entro al nivel", nombre);
}

void planificador_finalizar(tad_planificador* planificador){
	logger_info(planificador_logger(planificador), "Finalizando");
	//nombres mas cortos
	var(personajes, planificador->personajes);
	//liberamos los recursos de los datos de los personajes
	foreach(personaje, personajes, tad_personaje*){
		socket_close(personaje->socket);
		free(personaje->nombre);
		dealloc(personaje);
	}
	list_destroy(personajes);
	//liberamos los recursos de los datos del nivel
	socket_close(planificador->nivel->socket);
	dealloc(planificador->nivel);
	//liberamos los recursos propios del planificador
	logger_dispose_instance(planificador->logger);
	dealloc(planificador);
}
