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

#include "orquestador.h"

char* orquestador_puerto(tad_orquestador* orquestador){
	return orquestador->puerto; //TODO esto deberia se un atributo de socket_escucha
}
tad_logger* orquestador_logger(tad_orquestador* orquestador){
	return orquestador->logger;
}

tad_orquestador* orquestador_crear(tad_plataforma* plataforma){
	//alojamos una estructura tad_orquestador
	obj_alloc(ret, tad_orquestador);
	//seteamos la referencia a plataforma
	ret->plataforma = plataforma;
	//creamos una instancia del logger para el orquestador
	ret->logger = logger_new_instance("Orquestador");

	return ret;
}

void orquestador_ejecutar(tad_orquestador* orquestador){
	//creamos un socket de escucha y un multiplexor
	char* puerto = "27015";
	tad_socket* socket_escucha = socket_listen(puerto);
	tad_multiplexor* multiplexor = multiplexor_create();

	//asociamos el socket de escucha al multiplexor
	multiplexor_bind_socket(multiplexor, socket_escucha, orquestador_conexion_entrante, 1, orquestador);

	//guardamos las referencias en orquestador
	orquestador->puerto = puerto;
	orquestador->socket_escucha = socket_escucha;
	orquestador->multiplexor = multiplexor;

	//dejamos que el multiplexor haga el trabajo
	while(1)
		multiplexor_wait_for_io(multiplexor);
}


void orquestador_finalizar(tad_orquestador* orquestador){
	//cerramos todos los sockets y destruimos el multiplexor
	multiplexor_dispose_and_close_sockets(orquestador->multiplexor);

	//destruimos la instancia del logger
	logger_dispose_instance(orquestador->logger);
}






void orquestador_conexion_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);

	//nombres mas cortos
	var(socket_escucha, orquestador->socket_escucha);
	var(multiplexor, orquestador->multiplexor);

	//aceptamos la conexion entrante y le asociamos una nueva funcion manejadora
	tad_socket* socket_conexion = socket_accept_connection(socket_escucha);
	multiplexor_bind_socket(multiplexor, socket_conexion, orquestador_handshake, 2, orquestador, socket_conexion);
}

void orquestador_handshake(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	//nombres mas cortos
	var(multiplexor, orquestador->multiplexor);

	DECLARE_ERROR_MANAGER{
		//en caso de error cerramos el socket y lo desbindemos del multiplexor
		multiplexor_unbind_socket(multiplexor, socket);
		socket_close(socket);
		return;
	}FOR_SOCKET(socket);

	//enviamos un paquete de presentacion
	socket_send_empty_package(socket, PRESENTACION_ORQUESTADOR);

	//recibimos un paquete de presentacion
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PRESENTACION_NIVEL, PRESENTACION_PERSONAJE);
	byte tipo = package_get_data_type(paquete);
	package_dispose(paquete);

	//bindeamos el socket a una nueva funcion manejadora dependiendo del cliente
	switch(tipo){
	case PRESENTACION_NIVEL:
		multiplexor_rebind_socket(multiplexor, socket, orquestador_manejar_nivel, 2, orquestador, socket);
		break;
	case PRESENTACION_PERSONAJE:
		multiplexor_rebind_socket(multiplexor, socket, orquestador_manejar_personaje, 2, orquestador, socket);
		break;
	}
}

void orquestador_manejar_nivel(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	//nombres mas cortos
	var(multiplexor, orquestador->multiplexor);
	var(plataforma, orquestador->plataforma);

	//recibimos el numero del nivel que se conecto
	int nro_nivel = socket_receive_expected_int(socket, NIVEL_NUMERO);

	//nos fijamos si el planificador del nivel ya estaba iniciado
	if(plataforma_planificador_iniciado(plataforma, nro_nivel)){
		//si ya estaba iniciado, lo pateamos
		multiplexor_unbind_socket(socket);
		socket_close(socket);
	}else{
		//si no estaba iniciado, lo iniciamos
//		tad_planificador* planificador = plataforma_iniciar_planificador(plataforma, nro_nivel);
	}

	//TODO iniciar planificador, etc
}

void orquestador_manejar_personaje(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	//TODO derivar al planificador, etc
}
