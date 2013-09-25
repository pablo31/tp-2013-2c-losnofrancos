/*
 * variables.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/common.h"

struct my_struct{
	char* a;
	char* b;
};
typedef struct my_struct clase;

int main(void){

	obj_alloc(objeto, clase);
	objeto->a = "Texto A";
	objeto->b = "Texto B";

	var(referencia, objeto); //o bien ref(referencia, objeto)

	printf("%s\n%s\n", referencia->a, referencia->b);

	return EXIT_SUCCESS;
}
