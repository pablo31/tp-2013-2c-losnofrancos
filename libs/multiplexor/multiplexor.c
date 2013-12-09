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



class(phone){
	void* object;
	int (*id_getter)(void*);
	void (*destroyer)(void*);
	tad_command* command;
};



//Devuelve el puntero a la tabla de fds del multiplexor
private fd_set* multiplexor_get_master(tad_multiplexor* self){
	return &(self->master_set);
}

//Libera los recursos de un registro de llamada
private void multiplexor_dispose_phone(phone* p){
	command_dispose(p->command);
	dealloc(p);
}


//Crea un multiplexor
tad_multiplexor* multiplexor_create(){
	alloc(self, tad_multiplexor);

	//seteamos en cero la textura de fds
	fd_set* master = multiplexor_get_master(self);
	FD_ZERO(master);

	//creamos el diccionario de llamadas
	self->phone_book = list_create();

	//creamos un pipe para desbloquear el select
	pipe(self->pipe_fd);
	int pipe_read_fd = self->pipe_fd[0];
	FD_SET(pipe_read_fd, master);
	self->max_fd = pipe_read_fd;

	//seteamos la bandera que detiene el manejo de paquetes
	self->stop_io_handling = 0;

	return self;
}

//Agrega un objeto a la lista de escucha del multiplexor
void multiplexor_bind(tad_multiplexor* self,
		void* obj, int(*id_getter)(void*), void (*destroyer)(void*),
		void* handler_function, int numargs, ...){

	va_list inargs;
	va_start (inargs, numargs);
	tad_command* command = command_create_val(handler_function, numargs, inargs);

	int fd = id_getter(obj);
	fd_set* master = multiplexor_get_master(self);

	FD_SET(fd, master);
	self->max_fd = max(self->max_fd, fd);

	alloc(p, phone);
	p->object = obj;
	p->id_getter = id_getter;
	p->destroyer = destroyer;
	p->command = command;
	list_add(self->phone_book, p);
}

//Cambia la funcion manejadora de un objeto
void multiplexor_rebind(tad_multiplexor* self, void* obj, void* new_handler_function, int numargs, ...){
	va_list inargs;
	va_start (inargs, numargs);
	tad_command* new_command = command_create_val(new_handler_function, numargs, inargs);

	foreach(p, self->phone_book, phone*){
		if(p->object == obj){
			command_dispose(p->command);
			p->command = new_command;
			return;
		}
	}
}

//Itera en la lista de sockets y busca el mayor fd
private void multiplexor_refresh_max_fd(tad_multiplexor* self){
	int max_fd = 0;
	foreach(p, self->phone_book, phone*){
		int fd = p->id_getter(p->object);
		max_fd = max(max_fd, fd);
	}
	max_fd = max(max_fd, self->pipe_fd[0]);
	self->max_fd = max_fd;
}

//Quita un socket de la lista de escucha del multiplexor
void multiplexor_unbind(tad_multiplexor* self, void* obj){
	t_list* phone_book = self->phone_book;

	bool searched(void* p){ return ((phone*)p)->object == obj; }
	phone* p = list_remove_by_condition(phone_book, searched);

	int fd = p->id_getter(obj);
	FD_CLR(fd, multiplexor_get_master(self));

	multiplexor_dispose_phone(p);

	if(fd == self->max_fd) multiplexor_refresh_max_fd(self);
}

//Ejecuta el select, dado el parametro de tiempo maximo
private void multiplexor_execute_select(tad_multiplexor* self, struct timeval* tv){
	t_list* phone_book = self->phone_book;
	int opbs = list_size(phone_book); //original phone book size

	//creamos una copia de guia telefonica en un array
	phone phones[opbs];
	int i;
	for(i = 0; i < opbs; i++)
		phones[i] = *(phone*)(list_get(phone_book, i));

	//creamos una copia de la textura de fds
	int max_fd = self->max_fd + 1;
	fd_set* master_set = multiplexor_get_master(self);
	fd_set read_set = *master_set;

	//ejecutamos el nucleo del multiplexor
	select(max_fd, &read_set, null, null, tv); //bloqueante

	//limpiamos el pipe si es que llegaron datos a el
	int pipe_read_fd = self->pipe_fd[0];
	if(FD_ISSET(pipe_read_fd, &read_set)){
		char buf;
		read(pipe_read_fd, &buf, sizeof(buf));
	}

	//iteramos sobre el array buscando los sockets que recibieron mensajes
	for(i = 0; i < opbs; i++){
		//si se solicito que se detenga el manejo de paquetes entrantes
		if(self->stop_io_handling){
			self->stop_io_handling = 0;
			return;
		}
		//ejecutamos el manejador si corresponde
		phone p = phones[i];
		int fd = p.id_getter(p.object);
		if(FD_ISSET(fd, &read_set))
			command_execute(p.command);
	}
}

//Espera paquetes entrantes en los sockets asociados al multiplexor
void multiplexor_wait_for_io(tad_multiplexor* self){
	multiplexor_execute_select(self, null);
}

//Espera paquetes por un tiempo maximo determinado
void multiplexor_wait_for_io(tad_multiplexor* self, int ms){
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	multiplexor_execute_select(self, &tv);
}

//Espera paquetes por un tiempo maximo determinado, y devuelve el tiempo restante si un paquete llega antes
void multiplexor_wait_for_io(tad_multiplexor* self, int ms, int as_out remaining_ms){
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = ms * 1000;
	multiplexor_execute_select(self, &tv);
	set remaining_ms = tv.tv_usec / 1000;
}

//Libera los recursos del multiplexor
void multiplexor_dispose(tad_multiplexor* self){
	close(self->pipe_fd[0]);
	close(self->pipe_fd[1]);

	foreach(p, self->phone_book, phone*)
		multiplexor_dispose_phone(p);

	list_destroy(self->phone_book);
	dealloc(self);
}

//Libera los recursos del multiplexor y cierra todos sus sockets asociados
void multiplexor_dispose_and_dispose_objects(tad_multiplexor* self){
	foreach(p, self->phone_book, phone*)
		p->destroyer(p->object);

	multiplexor_dispose(self);
}

//Desbloquea automaticamente la proxima espera por paquetes entrantes
void multiplexor_simulate_io(tad_multiplexor* self){
	char buf = 0;
	write(self->pipe_fd[1], &buf, sizeof(buf));
}

//Detiene el manejo de paquetes entrantes
void multiplexor_stop_io_handling(tad_multiplexor* self){
	self->stop_io_handling = 1;
}
