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


private tad_thread* thread_new();
private void thread_dispose(tad_thread* thread);

private void* thread_command_execution(void* command_pointer);
private tad_thread* thread_begin_val(void* function, int numargs, va_list inargs);



/********************************************************
 * PUBLIC METHODS ***************************************
 ********************************************************/

/* Thread.begin
 * Inicia un nuevo hilo llamando a la funcion void especificada
 * Se debe incluir la cantidad de argumentos y, obviamente, los argumentos
 * Los argumentos luego deberan ser leidos en el orden en que fueron pasados
 *
 * Ejemplos:
 * 		tad_thread* hilo = thread_begin(f, 2, arg1, arg2);
 * 		tad_thread* hilo = thread_begin(f, 1, arg);
 * 		tad_thread* hilo = thread_begin(f, 0);
 * Declaracion y tipo obligatorio de la funcion invocada (si recibe argumentos):
 * 		void f(PACKED_ARGS)
 * Declaracion y tipo obligatorio de la funcion invocada (si no recibe argumentos):
 * 		void f(void)
 */
tad_thread* thread_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	return thread_begin_val(function, numargs, inargs);
}

/* Thread.freeBegin
 * Inicia un nuevo hilo sin guardarse la referencia a el
 * Permite iniciar el hilo y liberar sus recursos instantaneamente
 *
 * Ejemplos:
 * 		thread_free_begin(f, 2, arg1, arg2);
 * 		thread_free_begin(f, 1, arg);
 * 		thread_free_begin(f, 0);
 * Declaracion y tipo obligatorio de la funcion invocada (si recibe argumentos):
 * 		void f(PACKED_ARGS)
 * Declaracion y tipo obligatorio de la funcion invocada (si no recibe argumentos):
 * 		void f(void)
 */
void thread_free_begin(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	thread_dispose(thread_begin_val(function, numargs, inargs));
	return;
}

/* Thread.join
 * Detiene la ejecucion del hilo que llama a esta funcion
 * hasta que el hilo especificado termine de ejecutar
 */
void thread_join(tad_thread* thread){
	pthread_join(thread->thread, null);
	thread_dispose(thread);
}



/********************************************************
 * CREATION & DISPOSAL **********************************
 ********************************************************/

//Crea un tad_thread
private tad_thread* thread_new(){
	return malloc(sizeof(tad_thread));
}

//Libera los recursos de un thread
private void thread_dispose(tad_thread* thread){
	pthread_detach(thread->thread);
	free(thread);
}


/********************************************************
 * THREAD BEGIN & EXECUTION *****************************
 ********************************************************/

//Inicia una thread con los argumentos guardados en un va_list
private tad_thread* thread_begin_val(void* function, int numargs, va_list inargs){
	tad_command* command = command_create_val(function, numargs, inargs);

	tad_thread* ret = thread_new();
	pthread_create(&(ret->thread), null, thread_command_execution, (void*)command);

	return ret;
}

//Funcion donde inician los threads, ejecutando el command object
private void* thread_command_execution(void* command_pointer){
	tad_command* command = command_pointer;
	command_execute_and_dispose(command);
	return null;
}
