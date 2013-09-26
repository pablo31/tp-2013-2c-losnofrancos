/*
 * planificador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/common.h"

#include "planificador.h"

tad_planificador* planificador_crear(int nro_nivel, tad_socket* socket_nivel){
	//alojamos una estructura tad_planificador
	obj_alloc(ret, tad_planificador);
	//establecemos el numero de nivel correspondiente
	ret->nro_nivel = nro_nivel;
	//obtenemos una instancia del logger
	ret->logger = logger_new_instance("Planificador %d", nro_nivel);
	//establecemos el socket que lo comunica con nivel
	ret->socket_nivel = socket_nivel;

	logger_info(planificador_logger(ret), "Planificador del Nivel %d inicializado", nro_nivel);
	return ret;
}

int planificador_numero_nivel(tad_planificador* planificador){
	return planificador->nro_nivel;
}

tad_logger* planificador_logger(tad_planificador* planificador){
	return planificador->logger;
}
