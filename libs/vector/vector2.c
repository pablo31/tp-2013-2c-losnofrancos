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
	return vector2_new(1, 1);
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

vector2 vector2_add_x(vector2 v, int x){
	vector2 ret = v;
	ret.x += x;
	return ret;
}
vector2 vector2_add_y(vector2 v, int y){
	vector2 ret = v;
	ret.y += y;
	return ret;
}

vector2 vector2_direction_to(vector2 self, vector2 target){
	vector2 diff = vector2_subtract(target, self);
	diff.x /= abs(diff.x);
	diff.y /= abs(diff.y);
	return diff;
}

vector2 vector2_duplicate(vector2 v){
	return vector2_new(v.x, v.y);
}
