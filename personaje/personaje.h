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


//para que quiero esta estructura de aca abajo¿
//typedef struct {
//	char* nombre;
//	char  simbolo;
//	int   instancias;
//	vector2 posicion;
//} t_caja_Nivel;


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
} t_personaje;


#endif /* PERSONAJE_H_ */
