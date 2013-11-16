/*
 * vector2.h
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#ifndef VECTOR2_H_
#define VECTOR2_H_


#include "../overload.h"
#include "../common.h"

class(vector2){
	int x;
	int y;
};


#define vector2_new(args...) overload(vector2_new, args)

vector2 vector2_new(int x, int y);
vector2 vector2_new();

int vector2_get_x(vector2 v);
int vector2_get_y(vector2 v);
int vector2_equals(vector2 a, vector2 b);
vector2 vector2_add(vector2 a, vector2 b);
vector2 vector2_subtract(vector2 a, vector2 b);
vector2 vector2_dame_el_menor(vector2 a, vector2 b);
vector2 vector2_multiply(vector2 v, int s);
vector2 vector2_divide(vector2 v, int s);
vector2 vector2_add_x(vector2 v, int x);
vector2 vector2_add_y(vector2 v, int y);
vector2 vector2_direction_to(vector2 self, vector2 target);
vector2 vector2_duplicate(vector2 v);
int vector2_within_map(vector2 v, vector2 mapa);


#endif /* VECTOR2_H_ */
