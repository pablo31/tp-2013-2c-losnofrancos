/*
 * personaje.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PERSONAJE_H_
#define PERSONAJE_H_

#include "../libs/logger/logger.h"
#include "../libs/common/collections/list.h"
#include "../libs/collection/round.h"
#include "../libs/vector/vector2.h"


typedef struct {
	char* nombre;
	t_round* objetivos; //round<char*>
} t_nivel;

typedef struct {
	char* nombre;
	char simbolo;

	int vidas_iniciales;
	int vidas;
	t_list* niveles; //list<t_nivel>

	char* ippuerto_orquestador;
	tad_logger* logger;
	vector2 posicion; //el personaje sabe donde esta parado creo que por defecto entra en (1;1)
	vector2 posicionDelProximoRecurso;
} t_personaje;


#endif /* PERSONAJE_H_ */
