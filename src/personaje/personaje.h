/*
 * personaje.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */
#include "personaje_structs.h"


#ifndef PERSONAJE_H_
#define PERSONAJE_H_


typedef struct {
	char* nombre;
	t_connection_info* nivel;
	t_connection_info* planificador;
	t_socket_client* socket_nivel;
	t_socket_client* socket_planificador;
} t_personaje_nivel;

typedef struct {
	char* nombre;
	char simbolo;
	char** plan_de_niveles;
	t_dictionary* objetivos;
	int vidas;
	t_connection_info* orquestador_info;
	t_socket_client* socket_orquestador;
	int puerto;
	t_log* logger;
	t_personaje_nivel* nivel_actual;
	t_posicion* posicion;
	t_posicion* posicion_objetivo;
	bool nivel_finalizado;
	int nivel_actual_index;
	int vidas_iniciales;
	char** objetivos_array;
	char* objetivo_actual;
	int objetivo_actual_index;
	bool is_blocked;
} t_personaje;



t_personaje* personaje_create(char* config_path);
void personaje_destroy(t_personaje* self);
t_personaje_nivel* personaje_nivel_create(char* nombre_nivel);
void personaje_nivel_destroy(t_personaje_nivel* self);
t_dictionary* _personaje_load_objetivos(t_config* config,
		char** plan_de_niveles);

bool personaje_get_info_nivel(t_personaje* self);
bool personaje_conectar_a_orquestador(t_personaje* self);
bool personaje_conectar_a_nivel(t_personaje* self);
bool personaje_conectar_a_planificador(t_personaje* self);
bool personaje_jugar_nivel(t_personaje* self);
t_posicion* pedir_posicion_objetivo(t_personaje* self, char* objetivo);
bool realizar_movimiento(t_personaje* self);
bool mover_en_nivel(t_personaje* self);
bool finalizar_turno(t_personaje* self);
t_mensaje* solicitar_recurso(t_personaje* self);
void finalizar_nivel(t_personaje* self);
void personaje_avisar_fin_de_nivel(t_personaje* self);
void morir(t_personaje* self, char* motivo);
void avisar_muerte_a_nivel(t_personaje* self);


#endif /* PERSONAJE_H_ */
