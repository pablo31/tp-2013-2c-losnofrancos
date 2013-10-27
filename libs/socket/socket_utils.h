/*
 * socket_utils.h
 *
 *  Created on: Sep 13, 2013
 *      Author: pablo
 */

#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H_

#include "../vector/vector2.h"

#include "socket.h"

	/****************************************
	 * UTILS ********************************
	 ****************************************/

	//Crea y envia un paquete
	void socket_send(tad_socket* socket, byte data_type, int data_length, void* data);

	//Envia un paquete que no contiene datos
	void socket_send_empty_package(tad_socket* socket, byte data_type);
	//Recibe un paquete que no contiene datos
	byte socket_receive_empty_package(tad_socket* socket);
	//Recibe un paquete que no contiene datos, siempre que sea del tipo especificado
	void socket_receive_expected_empty_package(tad_socket* socket, byte data_type);

	//Recibe un paquete, siempre que sea del tipo especificador
	tad_package* socket_receive_expected_package(tad_socket* socket, byte data_type);
	//Recibe un paquete, siempre que sea de uno de los tipos especificados
	tad_package* socket_receive_one_of_this_packages(tad_socket* socket, int enum_length, ...);

	/****************************************
	 * COMMON DATA TYPES ********************
	 ****************************************/

	//Envia un paquete que contiene un string
	void socket_send_string(tad_socket* socket, byte data_type, char* string);
	//Recibe un paquete que contiene un string, siempre que sea del tipo especificado
	char* socket_receive_expected_string(tad_socket* socket, byte data_type);
	//Devuelve el string que conforma los datos de un paquete
	char* package_get_string(tad_package* package);

	#define socket_implement_common_h(type) \
		void socket_send_ ## type (tad_socket*, byte, type); \
		type socket_receive_expected_ ## type (tad_socket*, byte); \
		type package_get_ ## type (tad_package*)

	socket_implement_common_h(char);
	socket_implement_common_h(int);
	socket_implement_common_h(vector2);

	/****************************************
	 * MISC *********************************
	 ****************************************/

	//Obtiene la IP de un string del tipo IP:PUERTO
	char* string_get_ip(const char* ippuerto);
	//Obtiene el PUERTO de un string del tipo IP:PUERTO
	char* string_get_port(const char* ippuerto);
	//Genera el string del tipo IP:PUERTO a partir de la IP y el PUERTO
	char* string_to_ipport(const char* ip, const char* puerto);


#endif /* SOCKET_UTILS_H_ */
