/*
 * multiplexor.c
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/logger/logger.h"
#include "../libs/socket/socket.h"
#include "../libs/signal/signal.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/error/error_management.h"


private void conexion_entrante(PACKED_ARGS);
private void paquete_entrante(PACKED_ARGS);
private void cerrar_programa(PACKED_ARGS);


int main(void){
	printf("\n > Prueba del multiplexor\n");
	printf(" > \tMuestra como crear un multiplexor y asignarle sockets.\n");
	printf(" > \tSe le puede conectar cualquier cliente que trabaje con la libreria de sockets.\n");

	logger_initialize_for_info("prueba_multiplexor.log", "prueba_multiplexor");

	printf("Inicializando multiplexor...\n");
	tad_multiplexor* multiplexor = multiplexor_create();

	printf("Estableciendo señales...\n");
	signal_declare_handler(SIGINT, cerrar_programa, 1, multiplexor);

	printf("Inicializando socket...\n");
	tad_socket* socket_escucha = socket_listen("7666");
	multiplexor_bind_socket(multiplexor, socket_escucha, conexion_entrante, 2, multiplexor, socket_escucha);

	printf("Escuchando en el puerto 7666.\n");
	printf("Esperando conexiones...\n");

	while(1)
		multiplexor_wait_for_io(multiplexor); //bloqueante

	//No deberia llegar hasta aca
	return EXIT_FAILURE;
}


private void conexion_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_multiplexor* multiplexor);
	UNPACK_ARG(tad_socket* socket_escucha);

	//Aceptamos la conexion
	tad_socket* socket_conexion = socket_accept_connection(socket_escucha);
	printf("Nuevo cliente conectado.\n");

	//Bindeamos el socket al multiplexor
	multiplexor_bind_socket(multiplexor, socket_conexion, paquete_entrante, 2, multiplexor, socket_conexion);
}

private void paquete_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_multiplexor* multiplexor);
	UNPACK_ARG(tad_socket* socket);

	//Definimos un manejador de errores
	SOCKET_ERROR_MANAGER(socket){
		//En caso de desconexion...
		printf("Cliente desconectado.\n");
		multiplexor_unbind_socket(multiplexor, socket);
		socket_close(socket);
		return;
	}

	//Recibimos el paquete que estaba en espera
	tad_package* paquete = socket_receive_package(socket);

	char* texto = package_get_data(paquete);
	printf("Paquete recibido: tipo '%c', longitud %d, texto '%s'.\n",
			package_get_data_type(paquete),
			package_get_data_length(paquete),
			texto);

	//Liberamos sus recursos
	package_dispose(paquete);
	free(texto);
}


private void cerrar_programa(PACKED_ARGS){
	UNPACK_ARG(tad_multiplexor* multiplexor);

	//Cierro todas las conexiones y libero los recursos del multiplexor
	multiplexor_dispose_and_close_sockets(multiplexor);
	//Libero los recursos del logger
	logger_dispose();
	//Libero los recursos de las senales
	signal_dispose_all();

	printf("\nServer finalizado satisfactoriamente.\n\n");
	exit(EXIT_SUCCESS);
}
