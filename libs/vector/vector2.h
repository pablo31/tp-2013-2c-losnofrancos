/*
 * vector2.h
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#ifndef VECTOR2_H_
#define VECTOR2_H_

#include "../common.h"

#define EJE_X 0
#define EJE_Y 1


class(vector2){
	int x;
	int y;
};

//Overloads
#define vector2_new(args...) overload(vector2_new, args)

//Constructores
vector2 vector2_new(int x, int y);
vector2 vector2_new();

//Getters
int vector2_get_x(vector2 v);
int vector2_get_y(vector2 v);

//Comparacion
int vector2_equals(vector2 a, vector2 b);
int vector2_between(vector2 v, vector2 min, vector2 max);
int vector2_between_or_equals(vector2 v, vector2 min, vector2 max);
int vector2_within_map(vector2 v, vector2 mapa);

//Operaciones
vector2 vector2_add_x(vector2 v, int scalar);
vector2 vector2_add_y(vector2 v, int scalar);
vector2 vector2_add(vector2 a, vector2 b);
vector2 vector2_subtract(vector2 a, vector2 b);
vector2 vector2_multiply(vector2 v, int scalar);
vector2 vector2_divide(vector2 v, int scalar);

//Misc
vector2 vector2_minimize(vector2 a, vector2 b);
vector2 vector2_maximize(vector2 a, vector2 b);

//Metodos especificos del TP
vector2 vector2_direction_to(vector2 origin, vector2 target);
int vector2_distance_to(vector2 origin, vector2 target);
vector2 vector2_next_step(vector2 origin, vector2 target);
vector2 vector2_move_alternately(vector2 pos_salida, vector2 pos_llegada, int *eje_prox_mov);
int vector2_equals_xy(vector2 a, vector2 b, int eje);
int vector2_min_xy(vector2 a, vector2 b, int eje);
vector2 vector2_move_pos(vector2 posicion, int eje, int sentido);

//Metodos para SRDF
int vector2_distancia_escalar(vector2 origin, vector2 target);

#endif /* VECTOR2_H_ */
