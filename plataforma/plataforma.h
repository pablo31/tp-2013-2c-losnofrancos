/*
 * plataforma.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

#include "../libs/socket/socket.h"

/****************************
 * STRUCTS & TADS ***********
 ****************************/
struct s_plataforma;
typedef struct s_plataforma tad_plataforma;

#include "orquestador.h"
#include "planificador.h"

struct s_plataforma{
	tad_logger* logger;
	tad_orquestador* orquestador;
	t_list* planificadores;
};


/****************************
 * METHODS ******************
 ****************************/
tad_plataforma* plataforma_crear();
void plataforma_finalizar(tad_plataforma* plataforma);

tad_planificador* plataforma_planificador_iniciado(tad_plataforma* plataforma, char* nombre_nivel);
void plataforma_iniciar_planificador(tad_plataforma* plataforma, char* nombre_nivel, tad_socket* socket_nivel);
void plataforma_finalizar_planificador(tad_plataforma* plataforma, tad_planificador* planificador);


#endif /* PLATAFORMA_H_ */
