/*
 * cliente.c
 *
 *  Created on: May 1, 2013
 *      Author: pablo
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../libs/logger/logger.h"
#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/signal/signal.h"

void sigint_handler(PACKED_ARGS){
	UNPACK_ARG(tad_socket* socket);

	printf("\nLiberando recursos...\n");
	socket_close(socket);
	logger_dispose();
	signal_dispose_all();

	printf("Cliente finalizado satisfactoriamente.\n");
	exit(EXIT_SUCCESS);
}

int main(void){

	logger_initialize_for_debug("fruta", "prueba_cliente");

	printf("Pruebas > Cliente\nInicializando socket...\n");

	//Inicializo y me conecto al server
	tad_socket* socket = socket_connect("127.0.0.1", "7666");
	printf("Conectado\n");

	DECLARE_ERROR_MANAGER{
		printf("Se cerro la conexion de manera inesperada.\n");
		socket_close(socket);
		return EXIT_FAILURE;
	}FOR_SOCKET(socket);


	char input[255];

	while(1){
		printf(">> ");

		signal_declare_handler(SIGINT, sigint_handler, 1, socket);

		fgets(input, 255, stdin);
		int length = strlen(input);
		input[length - 1] = '\0'; //reemplazamos el \n por \0

		tad_package* paquete = package_create('s', strlen(input) + 1, input);
		socket_send_package(socket, paquete);
		printf("<< Texto enviado.\n");

		package_dispose(paquete);
	}

	//TODO hay un par de leaks por algun lado

	return EXIT_FAILURE;
}
