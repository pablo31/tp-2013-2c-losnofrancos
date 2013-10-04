/*
 * thread.c
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../command/command.h"
#include "../common.h"

#include "thread.h"


private tad_thread* thread_create();
private void thread_dispose(tad_thread* thread);

private void* thread_command_execution(void* command_pointer);
private tad_thread* thread_begin_val(void* function, int numargs, va_list inargs);



/********************************************************
 * PUBLIC METHODS ***************************************
 ********************************************************/

tad_thread* thread_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	return thread_begin_val(function, numargs, inargs);
}

void thread_free_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	thread_dispose(thread_begin_val(function, numargs, inargs));
	return;
}

void thread_join(tad_thread* thread){
	pthread_join(thread->thread, null);
	thread_dispose(thread);
}



/********************************************************
 * CREATION & DISPOSAL **********************************
 ********************************************************/

//Crea un tad_thread
private tad_thread* thread_create(){
	alloc(ret, tad_thread);
	return ret;
}

//Libera los recursos de un thread
private void thread_dispose(tad_thread* thread){
	pthread_detach(thread->thread);
	dealloc(thread);
}


/********************************************************
 * THREAD BEGIN & EXECUTION *****************************
 ********************************************************/

//Inicia una thread con los argumentos guardados en un va_list
private tad_thread* thread_begin_val(void* function, int numargs, va_list inargs){
	tad_command* command = command_create_val(function, numargs, inargs);

	tad_thread* ret = thread_create();
	pthread_create(&(ret->thread), null, thread_command_execution, (void*)command);

	return ret;
}

//Funcion donde inician los threads, ejecutando el command object
private void* thread_command_execution(void* command_pointer){
	tad_command* command = command_pointer;
	command_execute_and_dispose(command);
	return null;
}
