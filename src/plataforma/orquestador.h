/*
 * orquestador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef ORQUESTADOR_H_
#define ORQUESTADOR_H_

#include "plataforma.h"

struct s_orquestador{
	char* puerto;
	tad_socket* socket_escucha;
	tad_multiplexor* multiplexor;

	tad_plataforma* plataforma;
	tad_logger* logger;
};
typedef struct s_orquestador tad_orquestador;


//Getters
char* orquestador_puerto(tad_orquestador* orquestador);
tad_logger* orquestador_logger(tad_orquestador* orquestador);

//Inicializacion, ejecucion y destruccion
tad_orquestador* orquestador_crear(tad_plataforma* plataforma);
void orquestador_ejecutar(tad_orquestador* orquestador);
void orquestador_finalizar(tad_orquestador* orquestador);


#endif /* ORQUESTADOR_H_ */
