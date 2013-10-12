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


struct s_signal_command{
	int signal_id;
	tad_command* command;
};
typedef struct s_signal_command signal_command;



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

	if(command_list == null) command_list = list_create();

	int ssc(signal_command* sc){
		return sc->signal_id = signal_id;
	}

	signal_command* sc = list_find(command_list, (void*)ssc);
	if(sc == null){
		alloc(sc, signal_command);
		sc->signal_id = signal_id;
		sc->command = command;
		list_add(command_list, sc);
	}else{
		signal_replace_handler(sc, command);
	}

	signal(signal_id, signal_receive_function);
}

//Libera todos los recursos de las senales bindeadas
void signal_dispose_all(){

	void destroyer(void* psc){
		signal_command* sc = psc;
		command_dispose(sc->command);
		dealloc(sc);
	}

	list_destroy_and_destroy_elements(command_list, destroyer);
}
