/*
 * semaforo.c
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <pthread.h>

#include "../libs/common.h"

#include "mutex.h"

//Crea un semaforo listo para usar
tad_mutex* mutex_create(){
	alloc(ret, tad_mutex);
	alloc(pmutex, pthread_mutex_t);
	ret->pmutex = pmutex;
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
	dealloc(mutex->pmutex);
	dealloc(mutex);
}
