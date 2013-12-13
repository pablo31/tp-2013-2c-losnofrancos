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
#include "../libs/vector/vector2.h"
#include "../libs/common/string.h"
#include "../libs/thread/mutex.h"

typedef struct {
	char* nombre;
	t_list* objetivos; //list<char*>
} t_nivel;

typedef struct {
	char* nombre;
	char simbolo;

	int vidas_iniciales;
	int vidas;
	t_list* niveles; //list<t_nivel*>

	char* ippuerto_orquestador;
	tad_logger* logger;

	int auto_continue;

	tad_mutex* semaforo_sockets;
	t_list* sockets_bloqueados;
} t_personaje;



#endif /* PERSONAJE_H_ */
