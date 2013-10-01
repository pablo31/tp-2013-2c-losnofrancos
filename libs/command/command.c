/*
 * command.c
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */

#include "../common.h"

#include "command.h"


//Crea un command object
tad_command* command_create(void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	return command_create_val(function, numargs, inargs);
}

//Genera un tad_arguments a partir de una va_list
private tad_arguments* command_create_arguments_from_val(int numargs, va_list inargs){
	tad_arguments* arguments = null;

	if(numargs > 0){
		//Creamos la lista de argumentos
		arguments = arguments_new();
		int i;
		for(i = 0; i < numargs; i++){
			arguments_add(arguments, va_arg(inargs, void*));
		}
	}

	va_end(inargs);
	return arguments;
}

//Crea un command object a partir de una va_list
tad_command* command_create_val(void* function, int numargs, va_list inargs){
	//Creamos la lista de argumentos
	tad_arguments* arguments = command_create_arguments_from_val(numargs, inargs);

	//Creamos el objeto command
	alloc(ret, tad_command);
	ret->arguments = arguments;
	ret->function = function;

	return ret;
}

//Ejecuta una funcion
private void command_execute_function(void* function, tad_arguments* arguments){
	if(arguments == null){
		void(*call)(void) = function;
		call();
	}
	else{
		void(*call)(void*) = function;
		call((void*)arguments);
	}
}

//Ejecuta el comando
void command_execute(tad_command* command){
	command_execute_function(command->function, command->arguments);
}

//Ejecuta el comando liberando sus recursos
void command_execute_and_dispose(tad_command* command){
	tad_arguments* arguments = command->arguments;
	void* function = command->function;

	//Establecemos que los parametros liberen recursos automaticamente
	if(arguments != null) arguments_self_destroy(arguments);

	//Liberamos los recursos del puntero al command
	dealloc(command);

	//Ejecutamos la funcion
	command_execute_function(function, arguments);
}

//Devuelve el siguiente argumento empaquetado en el puntero a argumentos
void* get_next_argument(void* args_pointer){
	tad_arguments* arguments = args_pointer;
	return arguments_get(arguments);
}

//Libera recursos
void command_dispose(tad_command* command){
	arguments_dispose(command->arguments);
	dealloc(command);
}
