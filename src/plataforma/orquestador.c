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
	alloc(ret, tad_orquestador);
	//seteamos la referencia a plataforma
	ret->plataforma = plataforma;
	//creamos una instancia del logger para el orquestador
	ret->logger = logger_new_instance("Orquestador");

	logger_info(orquestador_logger(ret), "Orquestador inicializado");
	return ret;
}

void orquestador_ejecutar(tad_orquestador* orquestador){
	//creamos un socket de escucha y un multiplexor
	var(puerto, "27015");
	var(socket_escucha, socket_listen(puerto));
	var(multiplexor, multiplexor_create());
	//asociamos el socket de escucha al multiplexor
	multiplexor_bind_socket(multiplexor, socket_escucha, orquestador_conexion_entrante, 1, orquestador);

	//guardamos las referencias en orquestador
	orquestador->puerto = puerto;
	orquestador->socket_escucha = socket_escucha;
	orquestador->multiplexor = multiplexor;

	logger_info(orquestador_logger(orquestador), "Escuchando en el puerto %s", puerto);

	//dejamos que el multiplexor haga el trabajo
	while(1)
		multiplexor_wait_for_io(multiplexor);
}


void orquestador_finalizar(tad_orquestador* orquestador){
	//cerramos todos los sockets y destruimos el multiplexor
	multiplexor_dispose_and_close_sockets(orquestador->multiplexor);
	//destruimos la instancia del logger
	logger_dispose_instance(orquestador->logger);
	//liberamos memoria
	dealloc(orquestador);
}






void orquestador_conexion_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);

	//nombres mas cortos
	var(socket_escucha, orquestador->socket_escucha);
	var(multiplexor, orquestador->multiplexor);

	//aceptamos la conexion entrante
	tad_socket* socket_conexion = socket_accept_connection(socket_escucha);
	logger_info(orquestador_logger(orquestador), "Cliente conectado");
	//enviamos un paquete de presentacion
	socket_send_empty_package(socket_conexion, PRESENTACION_ORQUESTADOR);
	//asociamos el socket a una nueva funcion manejadora de clientes
	multiplexor_bind_socket(multiplexor, socket_conexion, orquestador_handshake, 2, orquestador, socket_conexion);
}

void orquestador_handshake(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	//nombres mas cortos
	var(multiplexor, orquestador->multiplexor);

	DECLARE_ERROR_MANAGER{
		switch(socket_get_error(socket)){
		case CONNECTION_CLOSED:
			logger_error(orquestador_logger(orquestador), "El cliente se desconecto inesperadamente");
			break;
		case UNEXPECTED_PACKAGE:
			logger_error(orquestador_logger(orquestador), "El cliente envio un paquete incorrecto");
			break;
		default:
			logger_error(orquestador_logger(orquestador), "Error en el envio o recepcion de datos del cliente");
			break;
		}
		//en caso de error cerramos el socket y lo desbindemos del multiplexor
		multiplexor_unbind_socket(multiplexor, socket);
		socket_close(socket);
		return;
	}FOR_SOCKET(socket);

	//recibimos un paquete de presentacion
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PRESENTACION_NIVEL, PRESENTACION_PERSONAJE);
	byte tipo = package_get_data_type(paquete);
	package_dispose(paquete);

	//bindeamos el socket a una nueva funcion manejadora dependiendo del cliente
	switch(tipo){
	case PRESENTACION_NIVEL:
		logger_info(orquestador_logger(orquestador), "El cliente es un Nivel");
		multiplexor_rebind_socket(multiplexor, socket, orquestador_manejar_nivel, 2, orquestador, socket);
		break;
	case PRESENTACION_PERSONAJE:
		logger_info(orquestador_logger(orquestador), "El cliente es un Personaje");
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
	logger_info(orquestador_logger(orquestador), "El cliente es el Nivel %d", nro_nivel);

	//nos fijamos si el planificador del nivel ya estaba iniciado
	if(plataforma_planificador_iniciado(plataforma, nro_nivel)){
		//si ya estaba iniciado, lo pateamos
		logger_error(orquestador_logger(orquestador), "El Planificador del Nivel %d ya estaba iniciado", nro_nivel);
		multiplexor_unbind_socket(multiplexor, socket);
		socket_close(socket);
	}else{
		//si no estaba iniciado, lo iniciamos
		logger_info(orquestador_logger(orquestador), "El Planificador del Nivel %d sera inicializado");
		plataforma_iniciar_planificador(plataforma, nro_nivel, socket);
		multiplexor_unbind_socket(multiplexor, socket);
	}
}

void orquestador_manejar_personaje(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);
	UNPACK_ARG(tad_socket* socket);

	//desbindeamos el socket del multiplexor ya que esta es la ultima comunicacion
	multiplexor_unbind_socket(orquestador->multiplexor, socket);

	//recibimos nombre y simbolo del personaje
	char* nombre = socket_receive_expected_string(socket, PERSONAJE_NOMBRE);
	char simbolo = socket_receive_expected_char(socket, PERSONAJE_SIMBOLO);
	logger_info(orquestador_logger(orquestador), "El cliente es el Personaje %s, con simbolo %c", nombre, simbolo);

	//recibimos su peticion de nivel o informe de objetivos completos
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PERSONAJE_SOLICITUD_NIVEL, PERSONAJE_OBJETIVOS_COMPLETADOS);
	byte tipo = package_get_data_type(paquete);
	package_dispose(paquete);

	switch(tipo){
	case PERSONAJE_SOLICITUD_NIVEL:
		orquestador_personaje_solicita_nivel(orquestador, socket, nombre, simbolo);
		break;
	case PERSONAJE_OBJETIVOS_COMPLETADOS:
		logger_info(orquestador_logger(orquestador), "El personaje %s informo que cumplio todos sus objetivos", nombre);
		//TODO
		break;
	}
}

void orquestador_personaje_solicita_nivel(tad_orquestador* orquestador, tad_socket* socket, char* nombre, char simbolo){
	//obtenemos el nro de nivel
	int nro_nivel = socket_receive_expected_int(socket, PERSONAJE_SOLICITUD_NIVEL);
	logger_info(orquestador_logger(orquestador), "El personaje %s solicito el nivel %d", nombre, nro_nivel);

	//nombres mas cortos
	var(plataforma, orquestador->plataforma);

	//nos fijamos si el planificador del nivel que pidio se encuentra iniciado
	var(planificador, plataforma_planificador_iniciado(plataforma, nro_nivel));
	if(planificador){
		//derivamos el personaje al planificador
		planificador_agregar_personaje(planificador, nombre, simbolo, socket);
	}else{
		//pateamos al personaje
		logger_error(orquestador_logger(orquestador), "El nivel que solicito el personaje %s no se encuentra iniciado", nombre);
		socket_close(socket);
	}
}
