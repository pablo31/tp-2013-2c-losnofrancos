/*
 * threads.c
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/thread/thread.h"
#include "../libs/common.h"


private int a; //variable global con threads......combinacion letal

private void foo(PACKED_ARGS){
	UNPACK_ARG(int* thread_numero);

	a++;
	printf("Thread numero %d > a=%d\n", *thread_numero, a);
}

int main(void){
	printf("\n > Prueba de ejecucion de hilos\n");
	printf(" > \tMuestra como crear y esperar la finalizacion de varios threads.\n");
	printf(" > \tPermite observar la planificacion desordenada de la libreria pthread.\n");

	printf("Prueba de threads iniciada.\n");

	a = 0;
	printf("Thread principal > a=0\n");

	int i = 1;
	tad_thread thread1 = thread_begin(foo, 1, &i);

	int ii = 2;
	tad_thread thread2 = thread_begin(foo, 1, &ii);

	int iii = 3;
	tad_thread thread3 = thread_begin(foo, 1, &iii);

	int iv = 4;
	tad_thread thread4 = thread_begin(foo, 1, &iv);

	thread_join(thread4);
	thread_join(thread3);
	thread_join(thread2);
	thread_join(thread1);

	printf("Prueba de threads finalizada.\n\n");
	return EXIT_SUCCESS;
}
