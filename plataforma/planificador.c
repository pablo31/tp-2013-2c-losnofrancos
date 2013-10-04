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

int planificador_numero_nivel(tad_planificador* self){
	return self->nivel->nro;
}

tad_logger* planificador_logger(tad_planificador* self){
	return self->logger;
}

void planificador_ejecutar(PACKED_ARGS){
	//UNPACK_ARG(tad_planificador* self);

	//TODO logica del planificador..
}

void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket){
	//alojamos una instancia de tad_personaje
	alloc(personaje, tad_personaje);
	personaje->nombre = nombre;
	personaje->simbolo = simbolo;
	personaje->socket = socket;
	//lo agregamos a la lista de personajes del planificador
	list_add(self->personajes, personaje);
	//informamos al usuario
	logger_info(planificador_logger(self), "El personaje %s entro al nivel", nombre);

	planificador_quitar_personaje(self, personaje); //TODO quitar esto cuando la logica este andando
}

void planificador_quitar_personaje(tad_planificador* self, tad_personaje* personaje){
	var(personajes, self->personajes);

	int i;
	for(i = 0; i < list_size(personajes); i++){
		tad_personaje* selected = list_get(personajes, i);
		if(selected == personaje){
			list_remove(personajes, i);
			socket_close(selected->socket);
			logger_info(planificador_logger(self), "El personaje %s fue pateado", selected->nombre);
			dealloc(selected);
			return;
		}
	}
}

void planificador_finalizar(tad_planificador* self){
	logger_info(planificador_logger(self), "Finalizando");
	//nombres mas cortos
	var(personajes, self->personajes);
	//liberamos los recursos de los datos de los personajes
	foreach(personaje, personajes, tad_personaje*)
		planificador_quitar_personaje(self, personaje);
	list_destroy(personajes);
	//liberamos los recursos de los datos del nivel
	//socket_close(self->nivel->socket); //TODO habilitar esto cuando los niveles se conecten
	dealloc(self->nivel);
	//liberamos los recursos propios del self
	logger_dispose_instance(self->logger);
	dealloc(self);
}
