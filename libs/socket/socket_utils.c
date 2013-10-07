/*
 * socket_utils.c
 *
 *  Created on: Sep 13, 2013
 *      Author: pablo
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../common/string.h"
#include "../common.h"

#include "socket_utils.h"


/****************************************
 * MISC METHODS *************************
 ****************************************/

//Devuelve la ip de un string del tipo ip:puerto
char* string_get_ip(const char* ipport){
	char copy[strlen(ipport)];
	strcpy(copy, ipport);

	char* ip = string_split(copy, ":")[0];

	return ip;
}

//Devuelve el puerto de un string del tipo ip:puerto
char* string_get_port(const char* ipport){
	char copy[strlen(ipport)];
	strcpy(copy, ipport);

	char* puerto = string_split(copy, ":")[1];

	return puerto;
}

//Devuelve un string del tipo ip:puerto
char* string_to_ipport(const char* ip, const char* port){
	char* ret = malloc(strlen(ip) + 1 + strlen(port));
	strcpy(ret, ip);
	strcat(ret, ":");
	strcat(ret, port);
	return ret;
}




/****************************************
 * PACKAGE UTILS ************************
 ****************************************/

//Crea y envia un paquete
void socket_send(tad_socket* socket, byte data_type, int data_length, void* data){
	tad_package* package = package_create(data_type, data_length, data);
	socket_send_package(socket, package);
	package_dispose(package);
}

//Recibe un paquete, pero arroja error si no es del tipo esperado
tad_package* socket_receive_expected_package(tad_socket* socket, byte data_type){
	tad_package* package = socket_receive_package(socket);

	if(package_get_data_type(package) != data_type){
		package_dispose(package);
		socket_set_error(socket, UNEXPECTED_PACKAGE);
		return null;
	}

	return package;
}






/****************************************
 * EMPTY PACKAGE UTILS ******************
 ****************************************/

//Envia un paquete que solo contiene el tipo de dato
void socket_send_empty_package(tad_socket* socket, byte data_type){
	socket_send(socket, data_type, 0, null);
}

//Recibe un paquete conformado solamente por el tipo de dato
byte socket_receive_empty_package(tad_socket* socket){
	tad_package* package = socket_receive_package(socket);

	if(package_get_data_length(package) > 0){
		package_dispose(package);
		socket_set_error(socket, UNEXPECTED_PACKAGE);
		return 0;
	}

	byte ret = package_get_data_type(package);
	package_dispose(package);
	return ret;
}




/****************************************
 * EXPECTED PACKAGE UTILS ***************
 ****************************************/

//Recibe un paquete, pero arroja error si no es del tipo esperado
void socket_receive_expected_empty_package(tad_socket* socket, byte data_type){
	byte value = socket_receive_empty_package(socket);

	if(value != data_type)
		socket_set_error(socket, UNEXPECTED_PACKAGE);
}

//Recibe un paquete, pero arroja error si no es uno de los tipos esperados
tad_package* socket_receive_one_of_this_packages(tad_socket* socket, int enum_length, ...){
	va_list inargs;
	va_start (inargs, enum_length);

	//recibimos un paquete
	tad_package* package = socket_receive_package(socket);

	int i;
	for(i = 0; i < enum_length; i++){
		//buscamos coincidencia con alguno de la lista
		int expected = va_arg(inargs, int);

		if(package_get_data_type(package) == expected){
			va_end(inargs);
			return package;
		}
	}

	//si no hubo coincidencias seteamos error
	va_end(inargs);
	package_dispose(package);
	socket_set_error(socket, UNEXPECTED_PACKAGE);
	return null;
}





private void* socket_receive_expected_data(tad_socket* socket, byte data_type){
	tad_package* package = socket_receive_expected_package(socket, data_type);
	return package_dispose_return_data(package);
}


//string

void socket_send_string(tad_socket* socket, byte data_type, char* string){
	socket_send(socket, data_type, strlen(string) + 1, string); //+1 por el \0
}

char* socket_receive_expected_string(tad_socket* socket, byte data_type){
	return (char*)socket_receive_expected_data(socket, data_type);
}

char* package_get_string(tad_package* package){
	return (char*)package_get_data(package);
}

//char

void socket_send_char(tad_socket* socket, byte data_type, char value){
	char* ptr = malloc(sizeof(char));
	*ptr = value;
	socket_send(socket, data_type, sizeof(char), ptr);
	free(ptr);
}

char socket_receive_expected_char(tad_socket* socket, byte data_type){
	char* ptr = (char*)socket_receive_expected_data(socket, data_type);
	char ret = *ptr;
	free(ptr);
	return ret;
}

//int

void socket_send_int(tad_socket* socket, byte data_type, int value){
	socket_send(socket, data_type, sizeof(int), &value);
}

int socket_receive_expected_int(tad_socket* socket, byte data_type){
	int* ptr = (int*)socket_receive_expected_data(socket, data_type);
	int ret = *ptr;
	free(ptr);
	return ret;
}

int package_get_int(tad_package* package){
	int* ptr = package_get_data(package);
	int ret = *ptr;
	return ret;
}
