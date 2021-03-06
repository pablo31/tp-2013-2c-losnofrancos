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
	char* koopa_cmd;
};


/****************************
 * METHODS ******************
 ****************************/
tad_plataforma* plataforma_crear();
void plataforma_liberar(tad_plataforma* self);
void plataforma_finalizar(tad_plataforma* plataforma);

tad_planificador* plataforma_planificador_iniciado(tad_plataforma* plataforma, char* nombre_nivel);
void plataforma_iniciar_planificador(tad_plataforma* plataforma, char* nombre_nivel, tad_socket* socket_nivel);
void plataforma_finalizar_planificador(tad_plataforma* plataforma, tad_planificador* planificador);
int plataforma_planificadores_vacios(tad_plataforma* self);
void un_personaje_termino_de_jugar(tad_plataforma* self);

#endif /* PLATAFORMA_H_ */
