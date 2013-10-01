/*
 * orquestador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef ORQUESTADOR_H_
#define ORQUESTADOR_H_

#include "../libs/multiplexor/multiplexor.h"
#include "../libs/socket/socket.h"
#include "../libs/logger/logger.h"
#include "../libs/common.h"

struct s_orquestador;
typedef struct s_orquestador tad_orquestador;

#include "plataforma.h"

struct s_orquestador{
	char* puerto;
	tad_socket* socket_escucha;
	tad_multiplexor* multiplexor;

	tad_plataforma* plataforma;
	tad_logger* logger;
};


//Getters
char* orquestador_puerto(tad_orquestador* orquestador);
tad_logger* orquestador_logger(tad_orquestador* orquestador);

//Inicializacion, ejecucion y destruccion
tad_orquestador* orquestador_crear(tad_plataforma* plataforma);
void orquestador_ejecutar(tad_orquestador* orquestador);
void orquestador_finalizar(tad_orquestador* orquestador);

//Manejo de conexiones entrantes
void orquestador_conexion_entrante(PACKED_ARGS);
void orquestador_handshake(PACKED_ARGS);
void orquestador_manejar_nivel(PACKED_ARGS);
void orquestador_manejar_personaje(PACKED_ARGS);
void orquestador_personaje_solicita_nivel(tad_orquestador* orquestador, tad_socket* socket, char* nombre, char simbolo);


#endif /* ORQUESTADOR_H_ */
