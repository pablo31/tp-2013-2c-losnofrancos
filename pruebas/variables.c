/*
 * variables.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/error/error_management.h"
#include "../libs/common.h"


struct my_struct{
	char* a;
	char* b;
};
typedef struct my_struct my_class;


void foo(int as_out i){
	set i = 2;
}

int main(void){
	printf("\n > Prueba de programacion a alto nivel\n");
	printf(" > \tMuestra como trabajar a mas alto nivel con los macros incluidos en libs.\n");
	printf(" > \tEntre ellos: instanciacion de clases, variables sin tipar, funciones con valores out, y bloque try-catch.\n");

	//alojamos una instancia de my_class
	alloc(my_obj, my_class);

	//seteamos sus atributos
	my_obj->a = "Struct Text A";
	my_obj->b = "Struct Text Z";

	//creamos variables sin tipar, referencias a distintos objetos
	var(reference, my_obj);
	var(hola, "123");

	//seteamos uno de los atributos de una referencia a my_obj
	reference->b = "Struct Text B";

	//obtenemos un valor out de una funcion
	int i;
	foo(out i);

	//imprimimos resultados en pantalla
	printf("%s\n%s\n%s\n%d\n", reference->a, reference->b, hola, i);

	//liberamos los recursos de my_obj
	dealloc(my_obj);

	//ejemplo de uso del bloque try catch
	TRY{
		printf("Entrando al bloque TRY\n");
		THROW(12);
		printf("Se arrojo excepcion 12 pero no fue atrapada!\n");
	}
	CATCH(12){
		printf("Atrapada excepcion 12!\n");
		THROW(75);
	}
	CATCH(75){
		printf("Atrapada excepcion 75!\n");
		THROW(99);
	}
	CATCH_OTHER{
		//para saber que excepcion atrapamos usamos el int __ex_num
		printf("Atrapada excepcion %d!\n", __ex_num);
	}

	return EXIT_SUCCESS;
}
