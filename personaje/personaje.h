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
	//estoy es un prototipo
	t_list* objetivosList;
	char* objetivo_actual;
	int objetivo_actual_index;
	bool nivel_finalizado;
	t_posicion* posicion;
	t_posicion* posicion_objetivo;
	// no tocar... jorge

//	char** plan_de_niveles;  // la idea es usar lista de lista,
//	t_dictionary* objetivos; //diccionario porque son fijos
//	t_connection_info* orquestador_info;   //pablo en algun lado tenes que
//	t_socket_client* socket_orquestador;   //tener estructuras como estas  =)
//	int puerto;
//	t_personaje_nivel* nivel_actual;
//	t_posicion* posicion;
//	t_posicion* posicion_objetivo;
//	bool nivel_finalizado;
//	int nivel_actual_index;
//	char** objetivos_array;
//	char* objetivo_actual;
//	int objetivo_actual_index;
//	bool is_blocked;
} t_personaje;



//t_personaje* personaje_create(char* config_path);  //ok
//void personaje_destroy(t_personaje* self);         //ok
//t_personaje_nivel* personaje_nivel_create(char* nombre_nivel);
//void personaje_nivel_destroy(t_personaje_nivel* self);
//t_dictionary* _personaje_load_objetivos(t_config* config,
//		char** plan_de_niveles);
//
////creo que es toda la funcionalidad de personaje, no se si me olvido de algo
//bool personaje_get_info_nivel(t_personaje* self);
//bool personaje_conectar_a_orquestador(t_personaje* self);
//bool personaje_conectar_a_nivel(t_personaje* self);
//bool personaje_conectar_a_planificador(t_personaje* self);
//bool personaje_jugar_nivel(t_personaje* self);
//t_posicion* pedir_posicion_objetivo(t_personaje* self, char* objetivo);
//bool realizar_movimiento(t_personaje* self);
//bool mover_en_nivel(t_personaje* self);
//bool finalizar_turno(t_personaje* self);
//t_mensaje* solicitar_recurso(t_personaje* self);
//void finalizar_nivel(t_personaje* self);
//void personaje_avisar_fin_de_nivel(t_personaje* self);
//void morir(t_personaje* self, char* motivo);
//void avisar_muerte_a_nivel(t_personaje* self);


#endif /* PERSONAJE_H_ */
