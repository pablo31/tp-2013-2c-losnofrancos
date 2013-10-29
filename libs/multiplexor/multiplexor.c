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
	void* object;
	int (*id_getter)(void*);
	void (*destroyer)(void*);
	tad_command* command;
} phone;


//Devuelve el puntero a la tabla de fds del multiplexor
private fd_set* multiplexor_get_master(tad_multiplexor* m){
	return &(m->master_set);
}

//Libera los recursos de un registro de llamada
private void multiplexor_dispose_phone(phone* p){
	command_dispose(p->command);
	dealloc(p);
}


//Crea un multiplexor
tad_multiplexor* multiplexor_create(){
	alloc(ret, tad_multiplexor);

	fd_set* master = multiplexor_get_master(ret);
	FD_ZERO(master);
	ret->max_fd = 0;

	ret->phone_book = list_create();

	return ret;
}

//Agrega un objeto a la lista de escucha del multiplexor
void multiplexor_bind(tad_multiplexor* m,
		void* obj, int(*id_getter)(void*), void (*destroyer)(void*),
		void* handler_function, int numargs, ...){

	va_list inargs;
	va_start (inargs, numargs);
	tad_command* command = command_create_val(handler_function, numargs, inargs);

	int fd = id_getter(obj);
	fd_set* master = multiplexor_get_master(m);

	FD_SET(fd, master);
	m->max_fd = max(m->max_fd, fd);

	alloc(p, phone);
	p->object = obj;
	p->id_getter = id_getter;
	p->destroyer = destroyer;
	p->command = command;
	list_add(m->phone_book, p);
}

//Cambia la funcion manejadora de un objeto
void multiplexor_rebind(tad_multiplexor* m, void* obj, void* new_handler_function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	tad_command* new_command = command_create_val(new_handler_function, numargs, inargs);

	foreach(p, m->phone_book, phone*){
		if(p->object == obj){
			command_dispose(p->command);
			p->command = new_command;
			return;
		}
	}
}

//Itera en la lista de sockets y busca el mayor fd
private void multiplexor_refresh_max_fd(tad_multiplexor* m){
	int max_fd = 0;
	foreach(p, m->phone_book, phone*){
		int fd = p->id_getter(p->object);
		max_fd = max(max_fd, fd);
	}
	m->max_fd = max_fd;
}

//Quita un socket de la lista de escucha del multiplexor
void multiplexor_unbind(tad_multiplexor* m, void* obj){
	t_list* phone_book = m->phone_book;

	bool searched(void* p){ return ((phone*)p)->object == obj; }
	phone* p = list_remove_by_condition(phone_book, searched);

	int fd = p->id_getter(obj);
	FD_CLR(fd, multiplexor_get_master(m));

	multiplexor_dispose_phone(p);

	if(fd == m->max_fd) multiplexor_refresh_max_fd(m);
}

//Ejecuta el select, dado el parametro de tiempo maximo
private void multiplexor_execute_select(tad_multiplexor* m, struct timeval* tv){
	t_list* phone_book = m->phone_book;
	int opbs = list_size(phone_book); //original phone book size

	//creamos una copia de guia telefonica en un array
	phone phones[opbs]; int i;
	for(i = 0; i < opbs; i++){
		phones[i] = *(phone*)(list_get(phone_book, i));
	}

	//creamos una copia de la textura de fds
	int max_fd = m->max_fd + 1;
	fd_set* master_set = multiplexor_get_master(m);
	fd_set read_set = *master_set;

	//ejecutamos el nucleo del multiplexor
	select(max_fd, &read_set, null, null, tv); //bloqueante

	//iteramos sobre el array buscando los sockets que recibieron mensajes
	for(i = 0; i < opbs; i++){
		phone p = phones[i];
		int fd = p.id_getter(p.object);
		//ejecutamos el manejador
		if(FD_ISSET(fd, &read_set))
			command_execute(p.command);
	}
}

//Espera paquetes entrantes en los sockets asociados al multiplexor
void multiplexor_wait_for_io(tad_multiplexor* m){
	multiplexor_execute_select(m, null);
}

//Espera paquetes por un tiempo maximo determinado
void multiplexor_wait_for_io(tad_multiplexor* m, int ms){
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	multiplexor_execute_select(m, &tv);
}

//Espera paquetes por un tiempo maximo determinado, y devuelve el tiempo restante si un paquete llega antes
void multiplexor_wait_for_io(tad_multiplexor* m, int ms, int as_out remaining_ms){
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	multiplexor_execute_select(m, &tv);
	set remaining_ms = tv.tv_usec;
}

//Libera los recursos del multiplexor
void multiplexor_dispose(tad_multiplexor* m){
	foreach(p, m->phone_book, phone*)
		multiplexor_dispose_phone(p);

	list_destroy(m->phone_book);
	dealloc(m);
}

//Libera los recursos del multiplexor y cierra todos sus sockets asociados
void multiplexor_dispose_and_dispose_objects(tad_multiplexor* m){
	foreach(p, m->phone_book, phone*)
		p->destroyer(p->object);

	multiplexor_dispose(m);
}
