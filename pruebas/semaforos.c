/*
 * semaforos.c
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/thread/thread.h"
#include "../libs/thread/mutex.h"
#include "../libs/common.h"


private int a;
private tad_mutex* semaforo;


private void foo(PACKED_ARGS){
	UNPACK_ARG(int* thread_numero);

	mutex_close(semaforo);

	a++;
	printf("Thread %d > a=%d\n", *thread_numero, a);

	mutex_open(semaforo);

	free(thread_numero);
}

private int* nuevo_numero(int numero){
	int* ret = malloc(sizeof(int));
	*ret = numero;
	return ret;
}

int main(void){
	printf("\n > Prueba de semaforos\n");
	printf(" > \tMuestra como crear y utilizar un semaforo para solucionar\n");
	printf(" > \tproblemas de concurrencia al trabajar con una variable global.\n");

	printf("Prueba de semaforos iniciada.\n");

	semaforo = mutex_create();

	a = 0;
	printf("Thread principal > a=0\n");

	int* i = nuevo_numero(1);
	tad_thread* thread1 = thread_begin(foo, 1, i);

	i = nuevo_numero(2);
	tad_thread* thread2 = thread_begin(foo, 1, i);

	i = nuevo_numero(3);
	tad_thread* thread3 = thread_begin(foo, 1, i);

	i = nuevo_numero(4);
	tad_thread* thread4 = thread_begin(foo, 1, i);

	thread_join(thread4);
	thread_join(thread3);
	thread_join(thread2);
	thread_join(thread1);

	mutex_dispose(semaforo);

	printf("Prueba de semaforos finalizada.\n\n");
	return EXIT_SUCCESS;
}
