/*
 * socket.h
 *
 *  Created on: 25/04/2013
 *      Author: pablo
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../logger/logger.h"
#include "../error/error_management.h"
#include "../common.h"


//by Pablo ;D


/****************************************
 * STRUCTS ******************************
 ****************************************/

//header
struct s_header{
	byte data_type;
	int data_length;
} __attribute__ ((__packed__));
typedef struct s_header tad_header;

//package
struct s_package{
	tad_header* header;
	void* data;
};
typedef struct s_package tad_package;

//socket error manager
struct s_error_manager{
	int error_number;
	byte code_jump_enabled;
	process_status* last_process_status;
};
typedef struct s_error_manager tad_error_manager;

//socket
struct s_socket{
	int socket_id;
	tad_logger* logger;
	tad_error_manager* error_manager;
};
typedef struct s_socket tad_socket;


/****************************************
 * ENUMS ********************************
 ****************************************/

enum SOCKET_ERR {
	NO_ERROR = 0,
	CONNECT_ERROR = 1,
	LISTEN_ERROR = 2,
	ACCEPT_CONNECTION_ERROR = 3,

	SEND_ERROR = 101,
	RECEIVE_ERROR = 102,
	UNEXPECTED_PACKAGE = 200,

	CONNECTION_CLOSED = 255
};


/****************************************
 * PACKAGE METHODS **********************
 ****************************************/
	//Creation
	tad_package* package_create(byte data_type, int data_length, void* data);
	//Getters
	byte package_get_data_type(tad_package* package);
	int package_get_data_length(tad_package* package);
	void* package_get_data(tad_package* package);
	//Disposal
	void package_dispose(tad_package* package);
	void* package_dispose_return_data(tad_package* package);
//	void package_dispose_copy_data(tad_package* package, void* mem_pos);


/****************************************
 * SOCKET METHODS ***********************
****************************************/
	//Getters
	int socket_get_id(tad_socket* socket); //try not to use this

	//Connection open
	tad_socket* socket_connect(char* ip, char* port);
	tad_socket* socket_listen(char* port);
	tad_socket* socket_accept_connection(tad_socket* socket);
	//Connection close
	void socket_close(tad_socket* socket);

	//Package transfer
	void socket_send_package(tad_socket* socket, tad_package* data);
	tad_package* socket_receive_package(tad_socket* socket);


/****************************************
 * SOCKET ERROR MANAGEMENT **************
****************************************/
	//Error getter & setters
	int socket_get_error(tad_socket* socket); //retorna 0 si no hay error
	void socket_set_error(tad_socket* socket, int error);
	void socket_reset_error(tad_socket* socket); //resetea el codigo de error

	//Program flux management
	void socket_bind_process_status(tad_socket* socket, process_status* ps);
	void socket_release_process_status(tad_socket* socket);

	//Error management block definition (see ../error/error_management.h)
	#define FOR_SOCKET(__r_md_socket) \
		socket_bind_process_status(__r_md_socket, &__r_md_ps)


#endif /* SOCKET_H_ */
