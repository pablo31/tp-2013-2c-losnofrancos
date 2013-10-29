/*
 * vector2.c
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#include <string.h>

#include "vector2.h"

vector2 vector2_new(int x, int y){
	vector2 ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

int vector2_get_x(vector2 v){
	return v.x;
}

int vector2_get_y(vector2 v){
	return v.y;
}

int vector2_equals(vector2 a, vector2 b){
	return (a.x == b.x) && (a.y == b.y);
}

vector2 vector2_add(vector2 a, vector2 b){
	vector2 ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	return ret;
}
vector2 vector2_subtract(vector2 a, vector2 b){
	vector2 ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	return ret;
}

vector2 vector2_direction_to(vector2 self, vector2 target){
	vector2 diff = vector2_subtract(target, self);
	diff.x /= abs(diff.x);
	diff.y /= abs(diff.y);
	return diff;
}

vector2 vector2_duplicate(vector2 posicion){
	return posicion_create(posicion->x, posicion->y);
}

//no lo termine...
#define DISTANCIA_MOVIMIENTO_PERMITIDA 1
vector2 posicion_desde_A_posicion_hacia(vector2 self, vector2 posicion_destino){

	if(vector2_equals(self, posicion_destino)){
		return vector2_duplicate(self);
	}

	int distancia_a_mover = DISTANCIA_MOVIMIENTO_PERMITIDA;

	int x = self->x;
	int y = self->y;

	//Esto hay que probarlo bien
	if (x != posicion_destino->x) {
		if (x < posicion_destino->x) {
			x += distancia_a_mover;
		} else {
			x -= distancia_a_mover;
		}
	} else if (y != posicion_destino->y) {
		if (y < posicion_destino->y) {
			y += distancia_a_mover;
		} else {
			y -= distancia_a_mover;
		}
	}

	return vector2_new(x,y);
}
