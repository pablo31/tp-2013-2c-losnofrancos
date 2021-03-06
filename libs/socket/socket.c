/*
 * socket.c
 *
 *  Created on: 25/04/2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../common/string.h"

#include "socket.h"

#define RECONNECT_INTERVAL 1

//by Pablo ;D

/* index:
 * -tad header methods
 * -tad package methods
 * -tad socket getters & setters
 * -tad socket creation, connection & disposal
 * -tad socket send
 * -tad socket receive
 */


/****************************************
 * HEADER *******************************
 ****************************************/

private tad_header* header_new(byte data_type, int data_length){
	alloc(ret, tad_header);
	ret->data_type = data_type;
	ret->data_length = data_length;

	return ret;
}

private void header_dispose(tad_header* header){
	dealloc(header);
}

/****************************************
 * PACKAGE ******************************
 ****************************************/

private tad_package* package_allocate(){
	alloc(ret, tad_package);
	return ret;
}

//Crea un paquete
tad_package* package_create(byte data_type, int data_length, void* data){
	tad_package* ret = package_allocate();
	tad_header* header = header_new(data_type, data_length);
	ret->header = header;
	ret->data = data;
	return ret;
}

private tad_package* package_create_from_header(tad_header* header, void* data){
	tad_package* ret = package_allocate();
	ret->header = header;
	ret->data = data;
	return ret;
}

//Devuelve el tipo de dato del paquete
byte package_get_data_type(tad_package* package){
	return package->header->data_type;
}

//Devuelve la longitud de los datos del paquete
int package_get_data_length(tad_package* package){
	return package->header->data_length;
}

//Devuelve los datos del paquete
void* package_get_data(tad_package* package){
	return package->data;
}

private tad_header* package_get_header(tad_package* package){
	return package->header;
}

//Destruye el paquete salvando los datos
void package_dispose(tad_package* package){
	header_dispose(package_get_header(package));
	dealloc(package);
}

//Destruye el paquete salvando los datos, y los retorna
void* package_dispose_return_data(tad_package* package){
	header_dispose(package_get_header(package));
	void* datos = package_get_data(package);
	dealloc(package);
	return datos;
}

//Destruye el paquete y sus datos
void package_dispose_all(tad_package* package){
	void* data = package_dispose_return_data(package);
	if(data != null) free(data);
}

/****************************************
 * GETTERS & SETTERS ********************
 ****************************************/

private void socket_initialize(tad_socket* socket, int socket_id){
	socket->socket_id = socket_id;
	socket->logger = logger_new_instance("Socket %d", socket_id);
	alloc(em, tad_error_manager);
	socket->error_manager = em;
	socket_release_process_status(socket);
	socket_reset_error(socket);
}

//Devuelve el numero descriptor del socket
private int socket_get_id(tad_socket* socket){
	return socket->socket_id;
}

//Devuelve el numero descriptor del socket (uso exclusivo del multiplexor)
int __socket_get_id(void* socket){
	return socket_get_id(socket);
}

tad_logger* socket_get_logger(tad_socket* socket){
	return socket->logger;
}

//Devuelve el estado de error del socket
int socket_get_error(tad_socket* socket){
	return socket->error_manager->error_number;
}

/****************************************
 * ERROR MANAGEMENT *********************
 ****************************************/

void socket_set_error(tad_socket* socket, int error){
	tad_error_manager* em = socket->error_manager;

	em->error_number = error;

	if(em->code_jump_enabled && error != 0)
		//Si se produjo un error de envio o recepcion de datos
		//saltamos al control de errores manipulando la call stack
		load_process_status(*(em->last_process_status));
}

//Elimina el estado de error del socket
void socket_reset_error(tad_socket* socket){
	socket_set_error(socket, NO_ERROR);
}

//Establece el inicio del control de errores para el socket
void socket_bind_process_status(tad_socket* socket, process_status* ps){
	socket->error_manager->last_process_status = ps;
	socket->error_manager->code_jump_enabled = 1;
}

//Establece que el socket no va a usar control de errores
void socket_release_process_status(tad_socket* socket){
	socket->error_manager->last_process_status = null;
	socket->error_manager->code_jump_enabled = 0;
}

/****************************************
 * CONNECT ******************************
 ****************************************/

private tad_socket* socket_new(){
	//obtengo un id libre
	int socket_id = socket(AF_INET, SOCK_STREAM, 0);

	//le digo al sistema que este id es reutilizable
	int optval = 1;
	setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	//creo el objeto socket
	alloc(ret, tad_socket);
	socket_initialize(ret, socket_id);

	logger_debug(socket_get_logger(ret), "Inicializado");
	return ret;
}

//Crea y conecta al socket a determinada ip y puerto
tad_socket* socket_connect(char* ip, char* port){
	tad_socket* socket = socket_new();

	struct sockaddr_in socket_info;

	memset(&socket_info, 0, sizeof(socket_info));
	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr(ip);
	socket_info.sin_port = htons(atoi(port));

	int error = 1;
	while(error!=0)
	{
		error = connect(socket_get_id(socket), (struct sockaddr*) &socket_info, sizeof(socket_info));
		if(error!=0) sleep(RECONNECT_INTERVAL);
	}

	logger_debug(socket_get_logger(socket), "Conectado a %s:%s", ip, port);
	
	return socket;
}

//Crea y pone al socket en escucha en determinado puerto
tad_socket* socket_listen(char* port){
	tad_socket* socket = socket_new();

	struct sockaddr_in socket_info;
	int socket_id = socket_get_id(socket);

	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = INADDR_ANY; //mi ip
	socket_info.sin_port = htons(atoi(port));
	bind(socket_id, (struct sockaddr*) &socket_info, sizeof(socket_info));
	//bind devuelve != 0 si hubo error

	int error = listen(socket_id, 10); //only god knows why 10
	
	if(error != 0){ //listen retorna != 0 si hubo error
		logger_error(socket_get_logger(socket), "Error al iniciar la escucha. (err id %d)", error);
		socket_set_error(socket, LISTEN_ERROR);
		return null;
	}

	logger_debug(socket_get_logger(socket), "Listo para escuchar");
	return socket;
}

//Espera y acepta una conexion entrante, derivandola a otro socket
tad_socket* socket_accept_connection(tad_socket* socket){
	int new_socket_id = accept(socket_get_id(socket), null, 0); //TODO cambiar null por estructura que me diga quien se conecto
	//accept retorna < 0 si hubo error
	if(new_socket_id < 0){
		logger_error(socket_get_logger(socket), "Error al aceptar la conexion. (err id %d)", new_socket_id);
		socket_set_error(socket, ACCEPT_CONNECTION_ERROR);
		return null;
	}

	alloc(new_socket, tad_socket);
	socket_initialize(new_socket, new_socket_id);

	logger_debug(socket_get_logger(new_socket), "Conexion entrante a socket %d aceptada", socket_get_id(socket));

	return new_socket;
}

//Cierra y elimina el socket
void socket_close(tad_socket* socket){
	close(socket_get_id(socket));
	logger_debug(socket_get_logger(socket), "Conexion cerrada");
	logger_dispose_instance(socket_get_logger(socket));
	dealloc(socket->error_manager);
	dealloc(socket);
}

void __socket_close(void* socket){
	socket_close(socket);
}

//Informa que se cerro la conexion inesperadamente y setea el error
private void socket_connection_closed(tad_socket* socket){
	logger_error(socket_get_logger(socket), "Se cerró la conexión de manera inesperada");
	socket_set_error(socket, CONNECTION_CLOSED);
}

/****************************************
 * SEND *********************************
 ****************************************/

private void socket_send_data(tad_socket* socket, int data_length, void* data){
	int error = send(socket_get_id(socket), data, data_length, MSG_NOSIGNAL);
	//send returns
	//-1 if error
	//else data length (>0)

	if(error == -1){
		free(data);
		socket_connection_closed(socket);
	}
	else if(error < 0){
		logger_error(socket_get_logger(socket), "Error al enviar datos serializados (err id %d)", error);
		free(data);
		socket_set_error(socket, SEND_ERROR);
	}
}

private void* socket_serialize(tad_package* package, int* out_data_length){
	int serialized_length = sizeof(tad_header) + package_get_data_length(package);
	void* serialized = malloc(serialized_length);

	memcpy(serialized, package_get_header(package), sizeof(tad_header));
	memcpy(serialized + sizeof(tad_header), package_get_data(package), package_get_data_length(package));

	*out_data_length = serialized_length;
	return serialized;
}

//Envia un paquete
void socket_send_package(tad_socket* socket, tad_package* package){

	int serialized_length;
	void* serialized = socket_serialize(package, &serialized_length);

	socket_send_data(socket, serialized_length, serialized);

	logger_debug(socket_get_logger(socket), "Paquete enviado - tipo %d - %d bytes de datos", package_get_data_type(package), package_get_data_length(package));

	free(serialized);
}


/****************************************
 * RECEIVE ******************************
 ****************************************/

private void* socket_receive_data(tad_socket* socket, int data_length){
	void* data = malloc(data_length);

	int error = recv(socket_get_id(socket), data, data_length, 0);
	//recv returns
	//-1 if error
	//0 if conection closed
	//else data length (>0)

	if(error < 0){
		logger_error(socket_get_logger(socket), "Error al recibir datos serializados. (err id %d)", error);
		free(data);
		socket_set_error(socket, RECEIVE_ERROR);
	}
	else if(error == 0){
		free(data);
		socket_connection_closed(socket);
	}

	return data;
}

private tad_header* socket_receive_header(tad_socket* socket){
	tad_header* header = socket_receive_data(socket, sizeof(tad_header));
	return header;
}

//Recibe un paquete
tad_package* socket_receive_package(tad_socket* socket){
	tad_header* header = socket_receive_header(socket);

	int data_length = header->data_length;

	void* data;
	if(data_length > 0) data = socket_receive_data(socket, data_length);
	else data = null;

	tad_package* package = package_create_from_header(header, data);

	logger_debug(socket_get_logger(socket), "Paquete recibido - tipo %d - %d bytes de datos", package_get_data_type(package), package_get_data_length(package));
	return package;
}

