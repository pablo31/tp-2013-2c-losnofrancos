/*
 * threads.c
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/thread/thread.h"
#include "../libs/thread/mutex.h"
#include "../libs/common.h"
#include "../libs/error/exception.h"


private int a; //variable global con threads......combinacion letal

private void problematic_function(int excepcion_a_arrojar){
	THROW(excepcion_a_arrojar);
}


private void path_to_problems(PACKED_ARGS){
	UNPACK_ARG(int* thread_numero);

	int excepcion_a_arrojar = 3 * (*thread_numero);

	TRY{
		printf("Thread %d > A punto de arrojar excepcion %d\n", *thread_numero, excepcion_a_arrojar);
		//llamamos a una funcion que arroja una excepcion
		problematic_function(excepcion_a_arrojar);
	}CATCH_OTHER{
		//para saber que excepcion atrapamos usamos la variable excno
		printf("Thread %d > Excepcion %d atrapada!\n", *thread_numero, excno);
	}
}

private void foo(PACKED_ARGS){
	UNPACK_ARG(int* thread_numero);

	a++;
	printf("Thread %d > a=%d\n", *thread_numero, a);
}

int main(void){
	printf("\n > Prueba de ejecucion de hilos\n");
	printf(" > \tMuestra como crear y esperar la finalizacion de varios threads.\n");
	printf(" > \tPermite observar la planificacion desordenada de la libreria pthread.\n");
	printf(" > \tImplementa excepciones para aplicaciones multi hilo.\n");

	printf("Prueba de threads iniciada.\n");

	a = 0;
	printf("Thread principal > a=0\n");

	int i = 1;
	tad_thread thread1 = thread_begin(path_to_problems, 1, &i);

	int ii = 2;
	tad_thread thread2 = thread_begin(foo, 1, &ii);

	int iii = 3;
	tad_thread thread3 = thread_begin(path_to_problems, 1, &iii);

	int iv = 4;
	tad_thread thread4 = thread_begin(foo, 1, &iv);

	int v = 5;
	tad_thread thread5 = thread_begin(path_to_problems, 1, &v);

	int vi = 6;
	tad_thread thread6 = thread_begin(foo, 1, &vi);

	thread_join(thread6);
	thread_join(thread5);
	thread_join(thread4);
	thread_join(thread3);
	thread_join(thread2);
	thread_join(thread1);

	printf("Prueba de threads finalizada.\n\n");
	return EXIT_SUCCESS;
}
