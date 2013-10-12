/*
 * personaje.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PERSONAJE_H_
#define PERSONAJE_H_


#include "../libs/common/collections/list.h"
#include "../libs/logger/logger.h"


//esto lo tiene andres, pero armo el mio, despues se hace un refactor..

typedef struct {
	int x;
	int y;
} t_posicion;

typedef struct {
	char* nombre;
	char  simbolo;
	int   instancias;
	t_posicion* posicion;
} t_caja_Nivel;


typedef struct {
	char* nombre;
	t_list* cajasPorNivel;
	//TODO agregar objetivos
} t_nivel;

typedef struct {
	char* nombre;
	char simbolo;
	tad_logger* logger;
	t_list* niveles;
	int vidas_iniciales;
	int vidas;
	char* ippuerto_orquestador;
	//no tocar... jorge  por el momento no lo termine..
	t_list* objetivosList;
	char* objetivo_actual;
	int objetivo_actual_index;
	t_posicion* posicion;
	t_posicion* posicion_objetivo;
	bool nivel_finalizado;
	bool completoTodosLosNiveles;
	bool is_blocked;
	tad_socket* socket_orquestador; // coneccion para conectar y desconectar
	// por el momento no tiene utilidad
	//no tocar... jorge

} t_personaje;


#endif /* PERSONAJE_H_ */
