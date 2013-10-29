
#include <stdbool.h>
#include <stdlib.h>

#include "../libs/common/collections/list.h"
#include "../libs/common/config.h"
#include "../libs/logger/logger.h"
#include "../libs/vector/vector2.h"

#ifndef NIVEL
#define NIVEL

typedef struct caja{
	char*	nombre;
	char	simbolo;
	int 	instancias;
	vector2 pos;
} caja;

typedef struct enemigo{
	char	simbolo;
	vector2 pos;
} enemigo;

typedef struct nivel {
	char*	 nombre;
	char*	 plataforma;
	uint 	 tiempo_deadlock;
	bool 	 recovery; // que hace este valor?
	t_list*     enemigos;
	uint     sleep_enemigos;
	char*	 algoritmo;
	int 	 retardo;
	int quantum;
	tad_logger* logger;
	t_list* cajas;
} nivel;

#endif
 
