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
#include "../libs/socket/socket.h"
#include "../libs/thread/thread.h"


class(t_nivel){
	char* nombre;
	t_list* objetivos; //list<char*>
};


class(t_personaje){
	char* nombre;
	char simbolo;

	int vidas_iniciales;
	int vidas;
	t_list* niveles; //list<t_nivel*>

	char* ippuerto_orquestador;
	tad_logger* logger;

	int auto_continue;
};


class(t_hilo){
	t_personaje* personaje;
	t_nivel* nivel;

	tad_thread thread;
	tad_logger* logger;

	tad_socket* socket;
	int bloqueado;
};



#endif /* PERSONAJE_H_ */
