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


//Creation
tad_command* command_create(void* function, int numargs, ...);
tad_command* command_create_val(void* function, int numargs, va_list inargs);

//Execution
void command_execute(tad_command* command);
void command_execute_and_dispose(tad_command* command);
void* get_next_argument(void* args_pointer);

//Disposal
void command_dispose(tad_command* command);


//Call Macros
#define PACKED_ARGS \
	void* __r_md_argsptr
#define UNPACK_ARG(__arg) \
	__arg = get_next_argument(__r_md_argsptr)


#endif /* COMMAND_H_ */
