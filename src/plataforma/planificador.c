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
	//establecemos el socket que lo comunica con nivel
	ret->socket_nivel = socket_nivel;

	return ret;
}

int planificador_numero_nivel(tad_planificador* planificador){
	return planificador->nro_nivel;
}
