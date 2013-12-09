/*
 * vector2.c
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#include <string.h>

#include "vector2.h"


/******************************
 * Constructores
 ******************************/


vector2 vector2_new(int x, int y){
	vector2 ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

vector2 vector2_new(){
	return vector2_new(0, 0);
}


/******************************
 * Getters
 ******************************/

int vector2_get_x(vector2 v){
	return v.x;
}

int vector2_get_y(vector2 v){
	return v.y;
}

int vector2_equals(vector2 a, vector2 b){
	return (a.x == b.x) && (a.y == b.y);
}


/******************************
 * Misc
 ******************************/

vector2 vector2_add_x(vector2 v, int scalar){
	v.x += scalar;
	return v;
}

vector2 vector2_add_y(vector2 v, int scalar){
	v.y += scalar;
	return v;
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

vector2 vector2_multiply(vector2 v, int scalar){
	vector2 ret;
	ret.x = v.x * scalar;
	ret.y = v.y * scalar;
	return ret;
}
vector2 vector2_divide(vector2 v, int scalar){
	vector2 ret;
	ret.x = v.x / scalar;
	ret.y = v.y / scalar;
	return ret;
}

int vector2_between(vector2 v, vector2 min, vector2 max){
	return v.x > min.x && v.y > min.y && v.x < max.x && v.y < max.y;
}

int vector2_between_or_equals(vector2 v, vector2 min, vector2 max){
	return v.x >= min.x && v.y >= min.y && v.x <= max.x && v.y <= max.y;
}

int vector2_within_map(vector2 v, vector2 map_limits){
	return vector2_between(v, vector2_new(0,0), map_limits);
}

vector2 vector2_minimize(vector2 a, vector2 b){
	vector2 ret;
	ret.x = a.x<b.x?a.x:b.x;
	ret.y = a.y<b.y?a.y:b.y;
	return ret;
}

vector2 vector2_maximize(vector2 a, vector2 b){
	vector2 ret;
	ret.x = a.x>b.x?a.x:b.x;
	ret.y = a.y>b.y?a.y:b.y;
	return ret;
}

vector2 vector2_direction_to(vector2 self, vector2 target){
	vector2 diff = vector2_subtract(target, self);
	diff.x /= abs(diff.x);
	diff.y /= abs(diff.y);
	return diff;
}

int vector2_distance_to(vector2 origin, vector2 target){
	vector2 delta = vector2_subtract(target, origin);
	int abs_x = abs(delta.x);
	int abs_y = abs(delta.y);
	return abs_x + abs_y;
}

vector2 vector2_next_step(vector2 origin, vector2 target){
	if(vector2_equals(origin, target)) return origin;

	vector2 delta = vector2_subtract(target, origin);
	int abs_x = abs(delta.x);
	int abs_y = abs(delta.y);

	vector2 movement;

	if(abs_x > abs_y)
		//hay mas distancia en x, nos movemos en x
		movement = vector2_new(delta.x / abs_x, 0);
	else
		//hay mas distancia en y, nos movemos en y
		movement = vector2_new(0, delta.y / abs_y);

	return vector2_add(origin, movement);
}


vector2 vector2_move_alternately(vector2 pos_salida, vector2 pos_llegada, int *eje_prox_mov){
	vector2 prox_pos;
	int eje = *eje_prox_mov;

    int mismo_eje = (vector2_equals_xy(pos_salida, pos_llegada, eje));

	if (mismo_eje)//si no puedo moverme en este eje
		eje = !eje; //me muevo en el mismo eje que antes

	int posicion_menor_en_eje = (vector2_min_xy(pos_salida, pos_llegada, eje));

	if (posicion_menor_en_eje){ //si la posicion actual en el eje es menor a la posicion de llegada
		prox_pos = vector2_move_pos(pos_salida, eje, 1); 	//me muevo hacia arriba o hacia la izquierda
	}
	else{
		prox_pos = vector2_move_pos(pos_salida, eje, -1); //me muevo hacia abajo o hacia la derecha
	}

	*eje_prox_mov = !eje; //seteo el eje alterno para el proximo movimiento

	return prox_pos;
}

int vector2_equals_xy(vector2 a, vector2 b, int eje){
	if (eje == EJE_X)
		return (a.x == b.x);
	else // EJE_Y
		return (a.y == b.y);
}


int vector2_min_xy(vector2 a, vector2 b, int eje){
	if (eje == EJE_X)
		return (a.x < b.x);
	else // EJE_Y
		return (a.y < b.y);
}

vector2 vector2_move_pos(vector2 posicion, int eje, int sentido){
	vector2 pos;
    if (eje == EJE_X)
    	pos = vector2_add_x(posicion, sentido);
    if (eje == EJE_Y)
    	pos = vector2_add_y(posicion, sentido);
    return pos;
}
