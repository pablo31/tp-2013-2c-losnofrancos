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

#include "../socket/socket.h"
#include "../common/collections/list.h"
#include "../command/command.h"


struct s_multiplexor{
	fd_set master_set; //fds texture
	int max_fd; //highest fd from fds texture
	t_list* phone_book; //socket-handler dictionary
};
typedef struct s_multiplexor tad_multiplexor;


//Creation
tad_multiplexor* multiplexor_create();

//Socket asociation
void multiplexor_bind_socket(tad_multiplexor* m, tad_socket* socket, void* handler_function, int numargs, ...);
void multiplexor_rebind_socket(tad_multiplexor* m, tad_socket* socket, void* new_handler_function, int numargs, ...);
void multiplexor_unbind_socket(tad_multiplexor* m, tad_socket* socket);

//Operations
void multiplexor_wait_for_io(tad_multiplexor* m);

//Disposal
void multiplexor_dispose(tad_multiplexor* m);
void multiplexor_dispose_and_close_sockets(tad_multiplexor* m);


#endif /* MULTIPLEXOR_H_ */
