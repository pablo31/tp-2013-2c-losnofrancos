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


//packed-args macros
#define PACKED_ARGS \
	void* __r_md_argsptr
#define UNPACK_ARG(__arg) \
	__arg = get_next_argument(__r_md_argsptr)


#include "../overload.h"
//overloaded packed-args macro
#define UNPACK_ARGS(...) \
	overload(UNPACK_ARGS, __VA_ARGS__)
//private macros
#define UNPACK_ARGS1(__arg) \
	UNPACK_ARG(__arg)
#define UNPACK_ARGS2(__arg1, __arg2) \
	UNPACK_ARG(__arg1); UNPACK_ARG(__arg2)
#define UNPACK_ARGS3(__arg1, __arg2, __arg3) \
	UNPACK_ARG(__arg1); UNPACK_ARG(__arg2); UNPACK_ARG(__arg3)
#define UNPACK_ARGS4(__arg1, __arg2, __arg3, __arg4) \
	UNPACK_ARG(__arg1); UNPACK_ARG(__arg2); UNPACK_ARG(__arg3); UNPACK_ARG(__arg4)
#define UNPACK_ARGS5(__arg1, __arg2, __arg3, __arg4, __arg5) \
	UNPACK_ARG(__arg1); UNPACK_ARG(__arg2); UNPACK_ARG(__arg3); UNPACK_ARG(__arg4); UNPACK_ARG(__arg5)

//#define UNPACK_ARGS(...) concat_hasargs(UNPACK_ARGS, __VA_ARGS__) (__VA_ARGS__)
//#define UNPACK_ARGS0(...)
//#define UNPACK_ARGS1(_arg, ...) UNPACK_ARG(__arg); UNPACK_ARGS(__VA_ARGS__)


#endif /* COMMAND_H_ */
