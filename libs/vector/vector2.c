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

void* vector2_serialize(vector2 v){
	int* x = &(v.x);
	int* y = &(v.y);
	int size = sizeof(int);
	void* dst = malloc(2 * size);
	memcpy(dst, x, size);
	memcpy(dst + size, y, size); //TODO
	return dst;
}

vector2 vector2_deserialize(void* src){
	int size = sizeof(int);
	vector2 ret;
	int* x = &(ret.x);
	int* y = &(ret.y);
	memcpy(x, src, size);
	memcpy(y, src + size, size); //TODO
	return ret;
}
