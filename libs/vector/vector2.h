/*
 * vector2.h
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#ifndef VECTOR2_H_
#define VECTOR2_H_

#define EJE_X 0
#define EJE_Y 1

#include "../common.h"


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
vector2 vector2_direction_to(vector2 self, vector2 target);
vector2 vector2_next_step(vector2 origen, vector2 destino);


vector2 vector2_move_in_L(vector2 enemigo_pos, int random, int cantidad);
vector2 moverse_una_posicion (vector2 posicion, int eje, int sentido);
vector2 movimiento_en_L (vector2 enemigo_pos, int eje, int sentido, int random, int cantidad);
int get_eje_alterno(int eje);


#endif /* VECTOR2_H_ */
