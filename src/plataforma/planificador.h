/*
 * planificador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "../libs/socket/socket.h"
#include "../libs/logger/logger.h"

struct s_planificador{
	int nro_nivel;
	tad_socket* socket_nivel;

	tad_logger* logger;
};
typedef struct s_planificador tad_planificador;

tad_planificador* planificador_crear(int nro_nivel, tad_socket* socket_nivel);
int planificador_numero_nivel(tad_planificador* planificador);
tad_logger* planificador_logger(tad_planificador* planificador);

#endif /* PLANIFICADOR_H_ */
