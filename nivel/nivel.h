#ifndef NIVEL
#define NIVEL


#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

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
	int detecta_personaje;
	vector2 posicion_personaje;
};


class(tad_personaje){
	char* nombre;
	char simbolo;
	vector2 pos;
	t_list* recursos_asignados;
	char recurso_pedido;
};


class (tad_recurso){
	char simbolo;
	int cantidad;
};


class(tad_nivel){
	char* nombre;
	tad_socket* socket;
	char* config_path;

	t_list* personajes;

	t_list* cajas;
	t_list* enemigos;

	char* algoritmo;
	int retardo;
	int quantum;

	uint tiempo_deadlock;
	bool recovery;
	uint sleep_enemigos;
	tad_logger* logger;
};


tad_logger* get_logger(tad_nivel* self);
char* get_config_path(tad_nivel* self);


#endif
 
