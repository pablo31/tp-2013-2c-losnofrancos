/*
 * signal.c
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */


#include <stdarg.h>
#include <stdlib.h>

#include "../common/collections/list.h"

#include "../command/command.h"
#include "../common.h"

#include "signal.h"



class(signal_command){
	int signal_id;
	tad_command* command;
};



private t_list* command_list;


private void signal_receive_function(int signal_id){
	foreach(sc, command_list, signal_command*){
		if(sc->signal_id == signal_id){
			command_execute(sc->command);
			return;
		}
	}
}

private void signal_replace_handler(signal_command* sc, tad_command* new_command_handler){
	command_dispose(sc->command);
	sc->command = new_command_handler;
}

//Bindea una funcion a una senal
void signal_declare_handler(int signal_id, void* function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);

	tad_command* command = command_create_val(function, numargs, inargs);

	if(!command_list) command_list = list_create();

	foreach(sc, command_list, signal_command*){
		if(sc->signal_id == signal_id){
			signal_replace_handler(sc, command);
			return;
		}
	}

	alloc(new_sc, signal_command);
	new_sc->signal_id = signal_id;
	new_sc->command = command;
	list_add(command_list, new_sc);
	signal(signal_id, signal_receive_function);
}

//Libera todos los recursos de las senales bindeadas
void signal_dispose_all(){

	foreach(sc, command_list, signal_command*){
		command_dispose(sc->command);
		dealloc(sc);
	}

	list_destroy(command_list);
}
