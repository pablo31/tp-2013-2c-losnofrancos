/*
 * planificador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "../libs/socket/socket.h"
#include "../libs/logger/logger.h"

struct s_personaje{
	char* nombre;
	char simbolo;
	tad_socket* socket;
};
typedef struct s_personaje tad_personaje;

struct s_nivel{
	int nro;
	tad_socket* socket;
};
typedef struct s_nivel tad_nivel;


struct s_planificador{
	tad_nivel* nivel;
	t_list* personajes;
	tad_logger* logger;
};
typedef struct s_planificador tad_planificador;


//Inicializacion
tad_planificador* planificador_crear(int nro_nivel, tad_socket* socket_nivel);
//Getters
int planificador_numero_nivel(tad_planificador* planificador);
tad_logger* planificador_logger(tad_planificador* planificador);
//Ejecucion
void planificador_ejecutar(PACKED_ARGS);
void planificador_finalizar(tad_planificador* planificador);
//Logica
void planificador_agregar_personaje(tad_planificador* planificador, char* nombre, char simbolo, tad_socket* socket);


#endif /* PLANIFICADOR_H_ */
