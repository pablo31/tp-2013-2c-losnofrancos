/*
 * command.h
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdarg.h>
#include <stdlib.h>

#include "arguments.h"


struct s_command{
	tad_arguments* arguments;
	void* function;
};
typedef struct s_command tad_command;


//Crea un command especificandole una funcion, la cantidad de argumentos, y los argumentos
tad_command* command_create(void* function, int numargs, ...);
//Crea un command desde una va_list
tad_command* command_create_val(void* function, int numargs, va_list inargs);

//Ejecuta el objeto command
void command_execute(tad_command* command);
//Ejecuta el objeto command y libera los recursos incluso antes de llamar a su funcion
void command_execute_and_dispose(tad_command* command);
//Devuelve el puntero al siguiente argumento del paquete de argumentos
void* get_next_argument(void* args_pointer); //try not to use this

//Libera los recursos del command
void command_dispose(tad_command* command);


//Call Macros
#define PACKED_ARGS \
	void* __r_md_argsptr
#define UNPACK_ARG(__arg) \
	__arg = get_next_argument(__r_md_argsptr)


#endif /* COMMAND_H_ */
