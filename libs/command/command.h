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

/*
#define UNPACK_FIVE_ARGS(arg1, arg2, arg3, arg4, arg5) \
	arg1 = get_next_argument(__r_md_argsptr); \
	arg2 = get_next_argument(__r_md_argsptr); \
	arg3 = get_next_argument(__r_md_argsptr); \
	arg4 = get_next_argument(__r_md_argsptr); \
	arg5 = get_next_argument(__r_md_argsptr)
#define UNPACK_FOUR_ARGS(arg1, arg2, arg3, arg4) \
	arg1 = get_next_argument(__r_md_argsptr); \
	arg2 = get_next_argument(__r_md_argsptr); \
	arg3 = get_next_argument(__r_md_argsptr); \
	arg4 = get_next_argument(__r_md_argsptr)
#define UNPACK_THREE_ARGS(arg1, arg2, arg3) \
	arg1 = get_next_argument(__r_md_argsptr); \
	arg2 = get_next_argument(__r_md_argsptr); \
	arg3 = get_next_argument(__r_md_argsptr)
#define UNPACK_TWO_ARGS(arg1, arg2) \
	arg1 = get_next_argument(__r_md_argsptr); \
	arg2 = get_next_argument(__r_md_argsptr)
#define UNPACK_ONE_ARG(arg1) \
	arg1 = get_next_argument(__r_md_argsptr)
*/
#define UNPACK_ARG(arg) \
	arg = get_next_argument(__r_md_argsptr)


#endif /* COMMAND_H_ */
