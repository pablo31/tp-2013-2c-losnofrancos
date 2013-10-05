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

/****************************
 * STRUCTS & TADS ***********
 ****************************/
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


/****************************
 * METHODS ******************
 ****************************/

//Crea una instancia de planificador
tad_planificador* planificador_crear(int nro_nivel, tad_socket* socket_nivel);
//Ejecuta el planificador (debe ser en un hilo exclusivo)
void planificador_ejecutar(PACKED_ARGS);
//Libera los recursos del planificador
void planificador_finalizar(tad_planificador* self);

//Agrega un personaje a la lista de juegadores del nivel
void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket);

//Devuelve el numero de nivel del planificador
int planificador_numero_nivel(tad_planificador* self);


#endif /* PLANIFICADOR_H_ */
