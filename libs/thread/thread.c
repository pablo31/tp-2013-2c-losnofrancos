/*
 * thread.c
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>

#include "../command/command.h"
#include "../common.h"

#include "thread.h"


private void* thread_command_execution(void* command_pointer);
private tad_thread thread_begin_val(void* function, int numargs, va_list inargs);



/********************************************************
 * PUBLIC METHODS ***************************************
 ********************************************************/

tad_thread thread_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	return thread_begin_val(function, numargs, inargs);
}

void thread_free_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	tad_thread thread = thread_begin_val(function, numargs, inargs);
	pthread_detach(thread);
	return;
}

void thread_join(tad_thread thread){
	pthread_join(thread, null);
	pthread_detach(thread);
}

void thread_kill(tad_thread thread){
	pthread_kill(thread, SIGKILL);
}

tad_thread thread_self(){
	return pthread_self();
}

int thread_equals(tad_thread a, tad_thread b){
	return pthread_equal(a, b);
}

tad_thread_key thread_create_variable(void(*destroyer)(void*)){
	pthread_key_t key;
	pthread_key_create(&key, destroyer);
	return key;
}

tad_thread_key thread_create_variable(){
	return thread_create_variable(null);
}

void* thread_get_variable(tad_thread_key key){
	return pthread_getspecific(key);
}

void thread_set_variable(tad_thread_key key, void* pointer){
	pthread_setspecific(key, pointer);
}


/********************************************************
 * THREAD BEGIN & EXECUTION *****************************
 ********************************************************/

//Inicia una thread con los argumentos guardados en un va_list
private tad_thread thread_begin_val(void* function, int numargs, va_list inargs){
	tad_command* command = command_create_val(function, numargs, inargs);

	tad_thread thread;
	pthread_create(&thread, null, thread_command_execution, (void*)command);

	return thread;
}

//Funcion donde inician los threads, ejecutando el command object
private void* thread_command_execution(void* command_pointer){
	tad_command* command = command_pointer;
	command_execute_and_dispose(command);
	return null;
}
