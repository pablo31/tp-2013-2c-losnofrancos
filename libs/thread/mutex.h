/*
 * semaforo.h
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <pthread.h>


typedef pthread_mutex_t tad_mutex;


//Crea un semaforo
tad_mutex* mutex_create();
//Cierra el semaforo
void mutex_close(tad_mutex* mutex);
//Abre el semaforo
void mutex_open(tad_mutex* mutex);
//Libera los recursos del semaforo
void mutex_dispose(tad_mutex* mutex);

//Semaforos estaticos
#define mutex_static PTHREAD_MUTEX_INITIALIZER


#endif /* SEMAFORO_H_ */
