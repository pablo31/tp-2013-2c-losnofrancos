/*
 * orquestador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"
#include "../protocol/protocol.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/common.h"

#include "orquestador.h"

char* orquestador_puerto(tad_orquestador* orquestador){
	return orquestador->puerto; //TODO esto deberia se un atributo de socket_escucha
}
tad_logger* orquestador_logger(tad_orquestador* orquestador){
	return orquestador->logger;
}

tad_orquestador* orquestador_crear(tad_plataforma* plataforma){
	obj_alloc(ret, tad_orquestador);
	ret->plataforma = plataforma;
	ret->logger = logger_new_instance("Orquestador");
	return ret;
}

void orquestador_ejecutar(tad_orquestador* orquestador){
	char* puerto = "27015";
	tad_socket* socket_escucha = socket_listen(puerto);
	tad_multiplexor* multiplexor = multiplexor_create();
	multiplexor_bind_socket(multiplexor, socket_escucha, orquestador_conexion_entrante, 1, orquestador);

	orquestador->puerto = puerto;
	orquestador->socket_escucha = socket_escucha;
	orquestador->multiplexor = multiplexor;

	while(1)
		multiplexor_wait_for_io(multiplexor);
}


void orquestador_finalizar(tad_orquestador* orquestador){
	multiplexor_dispose_and_close_sockets(orquestador->multiplexor);
	logger_dispose_instance(orquestador->logger);
}






void orquestador_conexion_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);

	var(socket_escucha, orquestador->socket_escucha);
	var(multiplexor, orquestador->multiplexor);

	tad_socket* socket_conexion = socket_accept_connection(socket_escucha);
	multiplexor_bind_socket(multiplexor, socket_conexion, orquestador_handshake, 2, orquestador, socket_conexion);
}

void orquestador_handshake(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	DECLARE_ERROR_MANAGER{
		multiplexor_unbind_socket(orquestador->multiplexor, socket);
		socket_close(socket);
		return;
	}FOR_SOCKET(socket);

	socket_send_empty_package(socket, PRESENTACION_ORQUESTADOR);

	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PRESENTACION_PERSONAJE, PRESENTACION_NIVEL);
	byte tipo = package_get_data_type(paquete);
	package_dispose(paquete);

	switch(tipo){
	case PRESENTACION_NIVEL:

		break;
	case PRESENTACION_PERSONAJE:

		break;
	}
}
