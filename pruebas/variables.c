/*
 * variables.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "../libs/error/error_management.h"
#include "../libs/error/exception.h"
#include "../libs/common.h"


/***************************************************************
 * Ejemplo de declaracion de una clase
 ***************************************************************/
class(my_class){
	char* a;
	char* b;
};


/***************************************************************
 * Ejemplo de funcion con sobrecarga en cantidad de parametros
 ***************************************************************/
#define min(args...) overload(min, args)

int min(){
	return 0;
}
int min(int a){
	return a;
}
int min(int a, int b){
	return a>b?b:a;
}
int min(int a, int b, int c){
	return min(a, min(b,c));
}


/***************************************************************
 * Ejemplo de funcion con parametros out
 ***************************************************************/
void foo(int as_out i){
	set i = 2;
}


/***************************************************************
 * Ejemplo de funcion que arroja una excepcion
 ***************************************************************/
void problematic_function(){
	printf("Funcion problematica arroja excepcion 69.\n");
	THROW(69);
}


/***************************************************************
 * Bloque Main
 ***************************************************************/
int main(void){
	printf("\n > Prueba de programacion a alto nivel\n");
	printf(" > \tMuestra como trabajar a mas alto nivel con los macros incluidos en libs.\n");
	printf(" > \tEntre ellos: instanciacion de clases, variables sin tipar, funciones con valores out, sobrecarga, y bloque try-catch.\n");

	//alojamos una instancia de my_class
	alloc(my_obj, my_class);

	//seteamos sus atributos
	my_obj->a = "Struct Field A";
	my_obj->b = "Struct Field B";

	//creamos variables sin tipar, referencias a distintos objetos
	var(reference, my_obj);
	var(hola, "123");

	//seteamos uno de los atributos de una referencia a my_obj
	reference->b = "Struct Field B Modified!";

	//obtenemos un valor out de una funcion
	int i;
	foo(out i);

	//llamamos a una funcion con sobrecarga
	int valor0 = min();
	int valor1 = min(5);
	int valor2 = min(10, 15);
	int valor3 = min(20, 25, 30);

	//imprimimos resultados en pantalla
	printf("%s\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n", reference->a, reference->b, hola, i, valor0, valor1, valor2, valor3);

	//liberamos los recursos de my_obj
	dealloc(my_obj);

	//ejemplo de uso del bloque try catch
	TRY{
		printf("Entrando a un bloque TRY\n");
		//llamamos a una funcion que arroja una excepcion
		problematic_function();
		printf("Se arrojo excepcion 12 pero no fue atrapada!\n");
	}
	CATCH(12){
		printf("Atrapada excepcion 12! Arrojada excepcion 99.\n");
		THROW(99);
	}
	CATCH(69){
		printf("Atrapada excepcion 69! Arrojada excepcion 12.\n");
		THROW(12);
	}
	CATCH_OTHER{
		//para saber que excepcion atrapamos usamos la variable excno
		printf("Atrapada excepcion %d!\n", excno);
	}

	return EXIT_SUCCESS;
}
