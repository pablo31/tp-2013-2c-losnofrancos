/*
 * plataforma.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

#include "../libs/socket/socket.h"

#include "orquestador.h"
#include "planificador.h"

struct s_plataforma{
	tad_logger* logger;
	tad_orquestador* orquestador;
	t_list* planificadores;
};
typedef struct s_plataforma tad_plataforma;


void plataforma_finalizar(PACKED_ARGS);

tad_plataforma* plataforma_crear();

tad_logger* plataforma_get_logger(tad_plataforma* plataforma);
tad_orquestador* plataforma_get_orquestador(tad_plataforma* plataforma);
t_list* plataforma_get_planificadores(tad_plataforma* plataforma);

int plataforma_planificador_iniciado(tad_plataforma* plataforma, int nro_nivel);
void plataforma_iniciar_planificador(tad_plataforma* plataforma, int nro_nivel, tad_socket* socket_nivel);


#endif /* PLATAFORMA_H_ */
