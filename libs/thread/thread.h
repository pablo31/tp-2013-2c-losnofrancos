/*
 * thread.h
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

#include "../command/command.h"
#include "../overload.h"

#define thread_create_variable(args...) overload(thread_create_variable, args)


typedef pthread_t tad_thread;


//Inicia un thread guardando una referencia a el (para luego hacer join)
tad_thread thread_begin(void* function, int numargs, ...);
//Inicia un thread sin guardar referencia a el
void thread_free_begin(void* function, int numargs, ...);
//Bloquea el programa hasta que el thread finalize
void thread_join(tad_thread thread);

//Devuelve el thread
tad_thread thread_self();
//Indica si dos threads son en realidad el mismo
int thread_equals(tad_thread a, tad_thread b);


typedef pthread_key_t tad_thread_key;

//Crea una variable thread-local alojada en el TLS
tad_thread_key thread_create_variable(void(*destroyer)(void*));
tad_thread_key thread_create_variable();
//Retorna la variable thread-local asociada a una key
void* thread_get_variable(tad_thread_key key);
//Setea la variable thread-local asociada a una key
void thread_set_variable(tad_thread_key key, void* pointer);


#endif /* THREAD_H_ */
