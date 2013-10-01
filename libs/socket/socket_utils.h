/*
 * socket_utils.h
 *
 *  Created on: Sep 13, 2013
 *      Author: pablo
 */

#ifndef SOCKET_UTILS_H_
#define SOCKET_UTILS_H_

#include "socket.h"

	/****************************************
	 * UTILS ********************************
	 ****************************************/

	//Package utils
	void socket_send(tad_socket* socket, byte data_type, int data_length, void* data);

	//Empty package utils
	void socket_send_empty_package(tad_socket* socket, byte data_type);
	byte socket_receive_empty_package(tad_socket* socket);
	void socket_receive_expected_empty_package(tad_socket* socket, byte data_type);

	//Expected package
	tad_package* socket_receive_expected_package(tad_socket* socket, byte data_type);
	tad_package* socket_receive_one_of_this_packages(tad_socket* socket, int enum_length, ...);

	/****************************************
	 * COMMON DATA TYPES ********************
	 ****************************************/

	//string
	void socket_send_string(tad_socket* socket, byte data_type, char* string);
	char* socket_receive_expected_string(tad_socket* socket, byte data_type);

	//char
	void socket_send_char(tad_socket* socket, byte data_type, char value);
	char socket_receive_expected_char(tad_socket* socket, byte data_type);

	//int
	void socket_send_int(tad_socket* socket, byte data_type, int value);
	int socket_receive_expected_int(tad_socket* socket, byte data_type);


	/****************************************
	 * MISC *********************************
	 ****************************************/

	char* string_get_ip(const char* ippuerto);
	char* string_get_port(const char* ippuerto);
	char* string_to_ipport(const char* ip, const char* puerto);


#endif /* SOCKET_UTILS_H_ */
