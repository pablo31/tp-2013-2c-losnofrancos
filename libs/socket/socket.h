/*
 * socket.h
 *
 *  Created on: 25/04/2013
 *      Author: pablo
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../logger/logger.h"
#include "../error/jump.h"
#include "../common.h"


//by Pablo ;D


/****************************************
 * STRUCTS ******************************
 ****************************************/

//header
class(tad_header){
	byte data_type;
	int data_length;
} __attribute__ ((__packed__));

//package
class(tad_package){
	tad_header* header;
	void* data;
};

//socket error manager
class(tad_error_manager){
	int error_number;
	byte code_jump_enabled;
	process_status* last_process_status;
};

//socket
class(tad_socket){
	int socket_id;
	tad_logger* logger;
	tad_error_manager* error_manager;
};


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

	CUSTOM_ERROR = 207,

	CONNECTION_CLOSED = 255
};


/****************************************
 * PACKAGE METHODS **********************
 ****************************************/
	//Crea un paquete
	tad_package* package_create(byte data_type, int data_length, void* data);
	//Devuelve el byte tipo de dato del paquete
	byte package_get_data_type(tad_package* package);
	//Devuelve la longitud de los datos del paquete
	int package_get_data_length(tad_package* package);
	//Devuelve un puntero a los datos del paquete
	void* package_get_data(tad_package* package);
	//Libera los recursos del paquete (excepto sus datos)
	void package_dispose(tad_package* package);
	//Libera los recursos del paquete (excepto sus datos) y devuelve un puntero a sus datos
	void* package_dispose_return_data(tad_package* package);


/****************************************
 * SOCKET METHODS ***********************
****************************************/
	//Crea y conecta un socket cliente a un servidor
	tad_socket* socket_connect(char* ip, char* port);
	//Crea y pone en escucha un socket servidor
	tad_socket* socket_listen(char* port);
	//Crea un socket y acepta (en el) la conexion entrante a un socket escucha servidor
	tad_socket* socket_accept_connection(tad_socket* socket);
	//Cierra la conexion y libera los recursos del socket
	void socket_close(tad_socket* socket);

	//Envia un paquete
	void socket_send_package(tad_socket* socket, tad_package* data);
	//Recibe un paquete
	tad_package* socket_receive_package(tad_socket* socket);

/****************************************
 * SOCKET MULTIPLEXOR METHODS ***********
****************************************/
	//Devuelve el FD del socket (uso exclusivo del multiplexor)
	int __socket_get_id(void* socket); //try not to use this
	//Cierra la conexion y libera los recursos del socket (uso exclusivo del multiplexor)
	void __socket_close(void* socket); //try not to use this

/****************************************
 * SOCKET ERROR MANAGEMENT **************
****************************************/
	//Devuelve el codigo de error del socket
	int socket_get_error(tad_socket* socket); //retorna 0 si no hay error
	//Setea el codigo de error del socket, y salta al manejo de errores si esta habilitado
	void socket_set_error(tad_socket* socket, int error);
	//Vuelve a 0 el codigo de error del socket
	void socket_reset_error(tad_socket* socket);

	//Asocia un bloque de manejo de errores al socket
	void socket_bind_process_status(tad_socket* socket, process_status* ps); //try not to use this
	//Libera el bloque de manejo de errores del socket
	void socket_release_process_status(tad_socket* socket); //try not to use this

	//Error management block definition (see ../error/error_management.h)
	#define SOCKET_ERROR_MANAGER(socket) \
		process_status __r_md_ps ## socket; \
		int __r_md_psr ## socket = save_process_status(__r_md_ps ## socket); \
		socket_bind_process_status(socket, &__r_md_ps ## socket); \
		if(__r_md_psr ## socket)

	//Error handler definition
	#define SOCKET_ON_ERROR(socket, call) \
		SOCKET_ERROR_MANAGER(socket){ \
			call; \
			return; \
		}

	//Error handler (with ret value) definition
	#define SOCKET_ON_ERROR_WRET(socket, call, ret) \
		SOCKET_ERROR_MANAGER(socket){ \
			call; \
			return ret; \
		}


#endif /* SOCKET_H_ */
