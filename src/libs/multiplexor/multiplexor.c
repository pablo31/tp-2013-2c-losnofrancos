/*
 * multiplexor.c
 *
 *  Created on: Sep 15, 2013
 *      Author: pablo
 */

#include <stdarg.h>

#include "../command/command.h"
#include "../common.h"

#include "multiplexor.h"




typedef struct s_phone{
	tad_socket* socket;
	tad_command* command;
} phone;


//Devuelve el puntero a la tabla de fds del multiplexor
private fd_set* multiplexor_get_master(tad_multiplexor* m){
	return &(m->master_set);
}

//Libera los recursos de un registro de llamada
private void multiplexor_dispose_phone(phone* p){
	command_dispose(p->command);
	free(p);
}


//Crea un multiplexor
tad_multiplexor* multiplexor_create(){
	tad_multiplexor* ret = malloc(sizeof(tad_multiplexor));

	fd_set* master = multiplexor_get_master(ret);
	FD_ZERO(master);
	ret->max_fd = 0;

	ret->phone_book = list_create();

	return ret;
}

//Agrega un socket a la lista de escucha del multiplexor
void multiplexor_bind_socket(tad_multiplexor* m, tad_socket* socket, void* handler_function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	tad_command* command = command_create_val(handler_function, numargs, inargs);

	int socket_id = socket_get_id(socket);
	fd_set* master = multiplexor_get_master(m);

	FD_SET(socket_id, master);
	m->max_fd = MAX(m->max_fd, socket_id);

	phone* p = malloc(sizeof(phone));
	p->socket = socket;
	p->command = command;
	list_add(m->phone_book, p);
}

//Cambia la funcion manejadora de un socket
void multiplexor_rebind_socket(tad_multiplexor* m, tad_socket* socket, void* new_handler_function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	tad_command* new_command = command_create_val(new_handler_function, numargs, inargs);

	foreach(p, m->phone_book, phone*){
		if(p->socket == socket){
			command_dispose(p->command);
			p->command = new_command;
			return;
		}
	}
}

//Itera en la lista de sockets y busca el mayor fd
private void multiplexor_refresh_max_fd(tad_multiplexor* m){
	int max = 0;
	foreach(p, m->phone_book, phone*){
		int socket_id = socket_get_id(p->socket);
		max = MAX(max, socket_id);
	}
	m->max_fd = max;
}

//Quita un socket de la lista de escucha del multiplexor
void multiplexor_unbind_socket(tad_multiplexor* m, tad_socket* socket){
	t_list* phone_book = m->phone_book;

	int i; //TODO este for anda¿?¿?¿
	for(i = 0; i < list_size(phone_book); i++){
		phone* p = list_get(phone_book, i);
		if(p->socket == socket){
			int socket_id = socket_get_id(socket);
			FD_CLR(socket_id, multiplexor_get_master(m));

			list_remove(phone_book, i);
			multiplexor_dispose_phone(p);

			if(socket_id == m->max_fd) multiplexor_refresh_max_fd(m);

			break;
		}
	}
}

//Escucha por paquetes entrantes en los sockets asociados, bloqueando la ejecucion
void multiplexor_wait_for_io(tad_multiplexor* m){
	int max_fd = m->max_fd + 1;
	fd_set* master_set = multiplexor_get_master(m);
	fd_set read_set = *master_set;

	select(max_fd, &read_set, null, null, null); //bloqueante

	//buscamos los sockets a los cuales les llegaron paquetes
	foreach(p, m->phone_book, phone*){
		int socket_id = socket_get_id(p->socket);

		//ejecutamos el manejador
		if(FD_ISSET(socket_id, &read_set))
			command_execute(p->command);
	}
}

//Libera los recursos del multiplexor
void multiplexor_dispose(tad_multiplexor* m){
	foreach(p, m->phone_book, phone*){
		multiplexor_dispose_phone(p);
	}

	list_destroy(m->phone_book);
	free(m);
}

//Libera los recursos del multiplexor y cierra todos sus sockets asociados
void multiplexor_dispose_and_close_sockets(tad_multiplexor* m){
	foreach(p, m->phone_book, phone*){
		socket_close(p->socket);
	}

	multiplexor_dispose(m);
}
