/*
 * vector2.h
 *
 *  Created on: Sep 30, 2013
 *      Author: pablo
 */

#ifndef VECTOR2_H_
#define VECTOR2_H_

#include "../common.h"

struct s_vector2{
	int x;
	int y;
};
typedef struct s_vector2 vector2;


vector2 vector2_new(int x, int y);
int vector2_get_x(vector2 v);
int vector2_get_y(vector2 v);
vector2 vector2_add(vector2 a, vector2 b);

void* vector2_serialize(vector2 v);
vector2 vector2_deserialize(void* src);


#endif /* VECTOR2_H_ */
