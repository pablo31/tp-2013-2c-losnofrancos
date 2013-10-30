#ifndef NIVEL
#define NIVEL


#include <stdbool.h>
#include <stdlib.h>

#include "../libs/common/collections/list.h"
#include "../libs/common/config.h"

#include "../libs/socket/socket.h"
#include "../libs/logger/logger.h"
#include "../libs/vector/vector2.h"
#include "../libs/common.h"


class(tad_caja){
	char* nombre;
	char simbolo;
	int instancias;
	vector2 pos;
};


class(tad_enemigo){
	char simbolo;
	vector2 pos;
};


class(tad_personaje){
	char simbolo;
	vector2 pos;
};


class(tad_nivel){
	char* nombre;
	tad_socket* socket;
	char* config_path;

	t_list* personajes; //list<tad_personaje>

	t_list* cajas; //list<tad_caja>
	t_list* enemigos; //list<tad_enemigo>

	char* algoritmo;
	int retardo;
	int quantum;

	uint tiempo_deadlock;
	bool recovery; // que hace este valor?
	uint sleep_enemigos;

	tad_logger* logger;
};


tad_logger* get_logger(tad_nivel* self);
char* get_config_path(tad_nivel* self);


#endif
 
