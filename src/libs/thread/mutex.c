/*
 * semaforo.c
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <pthread.h>

#include "mutex.h"

//Crea un semaforo listo para usar
tad_mutex* mutex_create(){
	tad_mutex* ret = malloc(sizeof(tad_mutex));
	ret->pmutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(ret->pmutex, NULL);
	return ret;
}

//Cierra el semaforo
void mutex_close(tad_mutex* mutex){
	pthread_mutex_t* pmutex = mutex->pmutex;
	pthread_mutex_lock(pmutex);
}

//Abre el semaforo
void mutex_open(tad_mutex* mutex){
	pthread_mutex_t* pmutex = mutex->pmutex;
	pthread_mutex_unlock(pmutex);
}

//Libera los recursos del semaforo
void mutex_dispose(tad_mutex* mutex){
	pthread_mutex_destroy(mutex->pmutex);
	free(mutex->pmutex);
	free(mutex);
}
