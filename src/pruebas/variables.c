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

	alloc_instance(my_class, my_obj);
	my_obj->a = "Struct Text A";
	my_obj->b = "Struct Text Z";

	var(reference, my_obj);
	var(function_result, foo(2));
	var(hola, "123");

	reference->b = "Struct Text B";

	printf("%s\n%s\n%d\n", reference->a, reference->b, function_result);

	free(my_obj);

	return EXIT_SUCCESS;
}
