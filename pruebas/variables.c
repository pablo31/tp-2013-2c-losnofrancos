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
typedef struct my_struct my_class;


int foo(){
	printf("Foo called!\n");
	return 2;
}

int main(void){

	//alojamos una instancia de my_class
	alloc(my_obj, my_class);

	//seteamos sus atributos
	my_obj->a = "Struct Text A";
	my_obj->b = "Struct Text Z";

	//creamos variables sin tipar, referencias a distintos objetos
	var(reference, my_obj);
	var(function_result, foo(2));
	var(hola, "123");

	//seteamos uno de los atributos de una referencia a my_obj
	reference->b = "Struct Text B";

	//imprimimos resultados en pantalla
	printf("%s\n%s\n%d\n%s\n", reference->a, reference->b, function_result, hola);

	//liberamos los recursos de my_obj
	dealloc(my_obj);

	return EXIT_SUCCESS;
}
