/*
 * semaforo.h
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <pthread.h>

struct s_mutex{
	pthread_mutex_t* pmutex;
};

typedef struct s_mutex tad_mutex;


//Creation
tad_mutex* mutex_create();

//Operation
void mutex_close(tad_mutex* mutex);
void mutex_open(tad_mutex* mutex);

//Disposal
void mutex_dispose(tad_mutex* mutex);


#endif /* SEMAFORO_H_ */
