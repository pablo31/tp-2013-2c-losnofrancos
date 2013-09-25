/*
 * plataforma.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

#include "orquestador.h"
#include "planificador.h"

struct s_plataforma{
	tad_logger* logger;
	tad_orquestador* orquestador;
	t_list* planificadores;
};
typedef struct s_plataforma tad_plataforma;


plataforma_planificador_iniciado(tad_plataforma* plataforma, int nro_nivel);


#endif /* PLATAFORMA_H_ */
