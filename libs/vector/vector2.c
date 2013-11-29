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

/*
vector2 vector2_move_in_L(vector2 enemigo_pos,int random, int cantidad){
	//padre nuestro que estas en los cielos santificado sea tu nombre......
	//perdona *sus* ifs

	switch (random) {

	case 0:
		if (cantidad==3){
			enemigo_pos.y ++;
		}else if (cantidad==2){
			enemigo_pos.y ++;
		}else if (cantidad==1){
			enemigo_pos.x --;
		}
		 break;
	case 1:
		if (cantidad==3){
			enemigo_pos.y ++;
		}else if (cantidad==2){
			enemigo_pos.y ++;
		}else if (cantidad==1){
			enemigo_pos.x ++;
		}
		break;
	case 2:
		if (cantidad==3){
			enemigo_pos.y --;
		}else if (cantidad==2){
			enemigo_pos.y --;
		}else if (cantidad==1){
			enemigo_pos.x ++;
		}
		break;
	case 3:
		if (cantidad==3){
			enemigo_pos.y --;
		}else if (cantidad==2){
			enemigo_pos.y --;
		}else if (cantidad==1){
			enemigo_pos.x --;
		}
		break;
	case 4:
		if (cantidad==3){
			enemigo_pos.x ++;
		}else if (cantidad==2){
			enemigo_pos.x ++;
		}else if (cantidad==1){
			enemigo_pos.y ++;
		}
		break;
	case 5:
		if (cantidad==3){
			enemigo_pos.x ++;
		}else if (cantidad==2){
			enemigo_pos.x ++;
		}else if (cantidad==1){
			enemigo_pos.y --;
		}
		break;
	case 6:
		if (cantidad==3){
			enemigo_pos.x --;
		}else if (cantidad==2){
			enemigo_pos.x --;
		}else if (cantidad==1){
			enemigo_pos.y ++;
		}
		break;
	case 7:
		if (cantidad==3){
			enemigo_pos.x --;
		}else if (cantidad==2){
			enemigo_pos.x --;
		}else if (cantidad==1){
			enemigo_pos.y --;
		}
		break;

	}

	return enemigo_pos;
}

*/

vector2 vector2_move_in_L(vector2 enemigo_pos, int random, int cantidad){

	switch (random) {

		case 0:
		case 1:

			//movimiento en L: me muevo dos posiciones en el eje Y hacia Abajo y una en el eje X (Izq. o Der.)
			movimiento_en_L(enemigo_pos, EJE_Y, 1, random, cantidad);
			break;

		case 2:
		case 3:

			//movimiento en L: me muevo dos posiciones en el eje Y hacia Arriba y una en el eje X (Izq. o Der.)
			movimiento_en_L(enemigo_pos, EJE_Y, -1, random, cantidad);
			break;

		case 4:
		case 5:

			//movimiento en L: me muevo dos posiciones en el eje X hacia la Derecha y una en el eje Y (Arriba o Abajo)
			movimiento_en_L(enemigo_pos, EJE_X, 1, random, cantidad);
			break;

		case 6:
		case 7:

			// movimiento en L: me muevo dos posiciones en el eje X hacia la Izquierda y una en el eje Y (Arriba o Abajo)
			movimiento_en_L(enemigo_pos, EJE_X, -1, random, cantidad);
    		break;

	}

	return enemigo_pos;

}


void moverse_una_posicion (vector2 posicion, int eje, int sentido){

    if (eje == EJE_X)
    	vector2_add_x(posicion, sentido);
    if (eje == EJE_Y)
    	vector2_add_x(posicion, sentido);

}


void movimiento_en_L (vector2 enemigo_pos, int eje, int sentido, int random, int cantidad){

	int eje_alterno;

	if (cantidad > 1){
		moverse_una_posicion (enemigo_pos, eje, sentido);
    }else{

    	eje_alterno = get_eje_alterno (eje);

    	if (random%2==0){
    		moverse_una_posicion (enemigo_pos, eje_alterno, 1);
    	}else{
    		moverse_una_posicion (enemigo_pos, eje_alterno, -1);
    	}

    }
}


int get_eje_alterno(int eje) {

	if (eje == EJE_X)
		return EJE_Y;
	else
		return EJE_X;

}



