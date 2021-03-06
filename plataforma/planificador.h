/*
 * planificador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "../libs/common/collections/queue.h"
#include "../libs/socket/socket.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/thread/mutex.h"
#include "../libs/logger/logger.h"
#include "../libs/vector/vector2.h"
#include "../libs/common.h"


/****************************
 * STRUCTS & TADS ***********
 ****************************/
class(tad_personaje){
	char* nombre;
	char simbolo;
	tad_socket* socket;

	vector2 pos;
	vector2 objetivo;
};

class(tad_nivel){
	char* nombre;
	tad_socket* socket;
};

struct s_planificador;
typedef struct s_planificador tad_planificador;

#include "plataforma.h"

struct s_planificador{
	tad_plataforma* plataforma;

	tad_nivel* nivel;
	tad_personaje* (*algoritmo)(tad_planificador*);
	int quantum;
	int retardo;

	tad_personaje* personaje_actual;
	int turnos_restantes;
	int solicito_ubicacion_recurso; //bandera hirroble para el unico mensaje sincronico

	t_list* personajes_listos; //list<tad_personaje>
	t_list* personajes_bloqueados; //list<tad_personaje>

	tad_mutex* semaforo; //semaforo para tocar la lista

	tad_multiplexor* multiplexor;

	tad_logger* logger;
};


/****************************
 * METHODS ******************
 ****************************/

//Crea una instancia de planificador
tad_planificador* planificador_crear(char* nombre_nivel, tad_socket* socket_nivel, tad_plataforma* plataforma);
//Ejecuta el planificador (debe ser en un hilo exclusivo)
void planificador_ejecutar(PACKED_ARGS);
//Libera los recursos del planificador
void planificador_finalizar(tad_planificador* self);

//Agrega un personaje a la lista de juegadores del nivel
void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket);

//Devuelve el numero de nivel del planificador
char* planificador_nombre_nivel(tad_planificador* self);

//Nos dice si el planificador esta vacio o sin personajes
int planificador_esta_vacio(tad_planificador* self);


#endif /* PLANIFICADOR_H_ */
