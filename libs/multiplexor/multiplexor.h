/*
 * multiplexor.h
 *
 *  Created on: Sep 15, 2013
 *      Author: pablo
 */

#ifndef MULTIPLEXOR_H_
#define MULTIPLEXOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/collections/list.h"
#include "../socket/socket.h"
#include "../command/command.h"

#include "../overload.h"


/***************************************************************
 * Class definition
 ***************************************************************/
class(tad_multiplexor){
	fd_set master_set;		//fds texture
	int max_fd;				//highest fd number from fds texture
	t_list* phone_book;		//socket-handler dictionary
};


/***************************************************************
 * Overloads
 ***************************************************************/
#define multiplexor_wait_for_io(args...) overload(multiplexor_wait_for_io, args)


/***************************************************************
 * Public methods
 ***************************************************************/

//Crea una instancia multiplexor
tad_multiplexor* multiplexor_create();

//Asocia un objeto y su funcion manejadora al multiplexor
void multiplexor_bind(tad_multiplexor* m,
		void* obj, int(*id_getter)(void*), void (*destroyer)(void*),
		void* handler_function, int numargs, ...);
//Cambia la funcion manejadora de un objeto ya asociado al multiplexor
void multiplexor_rebind(tad_multiplexor* m, void* obj, void* new_handler_function, int numargs, ...);
//Quita un objeto de la lista de asociados al multiplexor
void multiplexor_unbind(tad_multiplexor* m, void* obj);

//Espera paquetes entrantes en los sockets asociados al multiplexor
void multiplexor_wait_for_io(tad_multiplexor* m);
//Espera paquetes por un tiempo maximo determinado
void multiplexor_wait_for_io(tad_multiplexor* m, int ms);
//Espera paquetes por un tiempo maximo determinado, y devuelve el tiempo restante si un paquete llega antes
void multiplexor_wait_for_io(tad_multiplexor* m, int ms, int as_out remaining_ms);

//Libera los recursos del multiplexor
void multiplexor_dispose(tad_multiplexor* m);
//Libera los recursos del multiplexor y de todos sus sockets asociados
void multiplexor_dispose_and_dispose_objects(tad_multiplexor* m);


/***************************************************************
 * Socket methods
 ***************************************************************/
#define multiplexor_bind_socket(m, socket, handler, args...) \
	multiplexor_bind(m, socket, __socket_get_id, __socket_close, handler, VA_NUM_ARGS(args), args)

#define multiplexor_rebind_socket(m, socket, handler, args...) \
		multiplexor_rebind(m, socket, handler, VA_NUM_ARGS(args), args)

#define multiplexor_unbind_socket(m, socket) \
	multiplexor_unbind(m, socket);


#endif /* MULTIPLEXOR_H_ */
