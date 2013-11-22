/*
 * semaforo.c
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <pthread.h>

#include "../common.h"

#include "mutex.h"

//Crea un semaforo listo para usar
tad_mutex* mutex_create(){
	alloc(mutex, tad_mutex);
	pthread_mutex_init(mutex, null);
	return mutex;
}

//Cierra el semaforo
void mutex_close(tad_mutex* mutex){
	pthread_mutex_lock(mutex);
}

//Abre el semaforo
void mutex_open(tad_mutex* mutex){
	pthread_mutex_unlock(mutex);
}

//Libera los recursos del semaforo
void mutex_dispose(tad_mutex* mutex){
	pthread_mutex_destroy(mutex);
	dealloc(mutex);
}
