/*
 * orquestador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/multiplexor/multiplexor.h"

#include "../libs/protocol/protocol.h"

#include "orquestador.h"



private char* get_puerto(tad_orquestador* orquestador){
	return orquestador->puerto; //TODO esto deberia se un atributo de socket_escucha
}
private tad_logger* get_logger(tad_orquestador* orquestador){
	return orquestador->logger;
}


tad_orquestador* orquestador_crear(tad_plataforma* plataforma, char* puerto){
	//alojamos una estructura tad_orquestador
	alloc(ret, tad_orquestador);
	//seteamos la referencia a plataforma
	ret->plataforma = plataforma;
	//creamos una instancia del logger para el orquestador
	ret->logger = logger_new_instance("Orquestador");
	//seteamos el puerto donde escucha el orquestador
	ret->puerto = puerto;

	logger_info(get_logger(ret), "Orquestador inicializado");
	return ret;
}

void orquestador_ejecutar(tad_orquestador* self){
	//creamos un socket de escucha y un multiplexor
	var(puerto, get_puerto(self));
	var(socket_escucha, socket_listen(puerto));
	var(multiplexor, multiplexor_create());

	//asociamos el socket de escucha al multiplexor
	multiplexor_bind_socket(multiplexor, socket_escucha, orquestador_conexion_entrante, self);

	//guardamos las referencias en self
	self->socket_escucha = socket_escucha;
	self->multiplexor = multiplexor;

	logger_info(get_logger(self), "Escuchando en el puerto %s", puerto);

	//dejamos que el multiplexor haga el trabajo
	while(1)
		multiplexor_wait_for_io(multiplexor);
}

void orquestador_finalizar(tad_orquestador* self){
	//cerramos todos los sockets y destruimos el multiplexor
	multiplexor_dispose_and_dispose_objects(self->multiplexor);
	//destruimos la instancia del logger
	logger_info(get_logger(self), "Finalizando");
	logger_dispose_instance(self->logger);
	//liberamos memoria
	free(self->puerto);
	dealloc(self);
}

void orquestador_conexion_entrante(PACKED_ARGS){
	UNPACK_ARG(tad_orquestador* orquestador);

	//nombres mas cortos
	var(socket_escucha, orquestador->socket_escucha);
	var(multiplexor, orquestador->multiplexor);

	//aceptamos la conexion entrante
	tad_socket* socket_conexion = socket_accept_connection(socket_escucha);
	logger_info(get_logger(orquestador), "Cliente conectado");
	//enviamos un paquete de presentacion
	socket_send_empty_package(socket_conexion, PRESENTACION_ORQUESTADOR);
	//asociamos el socket a una nueva funcion manejadora de clientes
	multiplexor_bind_socket(multiplexor, socket_conexion, orquestador_handshake, orquestador, socket_conexion);
}

void orquestador_cliente_desconectado(tad_orquestador* orquestador, tad_socket* socket){
	switch(socket_get_error(socket)){
	case CONNECTION_CLOSED:
		logger_error(get_logger(orquestador), "El cliente se desconecto inesperadamente");
		break;
	case UNEXPECTED_PACKAGE:
		logger_error(get_logger(orquestador), "El cliente envio un paquete incorrecto");
		break;
	default:
		logger_error(get_logger(orquestador), "Error en el envio o recepcion de datos del cliente");
		break;
	}
	//cerramos el socket y lo desbindemos del multiplexor
	multiplexor_unbind_socket(orquestador->multiplexor, socket);
	socket_close(socket);
}

void orquestador_handshake(PACKED_ARGS){
	UNPACK_ARGS(tad_orquestador* orquestador, tad_socket* socket);

	//establecemos la funcion manejadora de desconexion del socket
	SOCKET_ON_ERROR(socket, orquestador_cliente_desconectado(orquestador, socket));

	//nombres mas cortos
	var(multiplexor, orquestador->multiplexor);

	//recibimos un paquete de presentacion
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PRESENTACION_NIVEL, PRESENTACION_PERSONAJE);
	byte tipo = package_get_data_type(paquete);
	package_dispose(paquete);

	//bindeamos el socket a una nueva funcion manejadora dependiendo del cliente
	switch(tipo){
	case PRESENTACION_NIVEL:
		logger_info(get_logger(orquestador), "El cliente es un Nivel");
		multiplexor_rebind_socket(multiplexor, socket, orquestador_manejar_nivel, orquestador, socket);
		break;
	case PRESENTACION_PERSONAJE:
		logger_info(get_logger(orquestador), "El cliente es un Personaje");
		multiplexor_rebind_socket(multiplexor, socket, orquestador_manejar_personaje, orquestador, socket);
		break;
	}
}

void orquestador_manejar_nivel(PACKED_ARGS){
	UNPACK_ARGS(tad_orquestador* self, tad_socket* socket);

	//establecemos la funcion manejadora de desconexion del socket
	SOCKET_ON_ERROR(socket, orquestador_cliente_desconectado(self, socket));

	//nombres mas cortos
	var(multiplexor, self->multiplexor);
	var(plataforma, self->plataforma);

	//recibimos el numero del nivel que se conecto
	char* nombre = socket_receive_expected_string(socket, NIVEL_NUMERO);
	logger_info(get_logger(self), "El cliente es el Nivel %s", nombre);

	//nos fijamos si el planificador del nivel ya estaba iniciado
	if(plataforma_planificador_iniciado(plataforma, nombre)){
		//si ya estaba iniciado, lo pateamos
		logger_error(get_logger(self), "El Planificador del Nivel %s ya estaba iniciado", nombre);
		multiplexor_unbind_socket(multiplexor, socket);
		socket_close(socket);
	}else{
		//si no estaba iniciado, lo iniciamos
		logger_info(get_logger(self), "El Planificador del Nivel %s sera inicializado", nombre);
		plataforma_iniciar_planificador(plataforma, nombre, socket);
		multiplexor_unbind_socket(multiplexor, socket);
	}
}

void orquestador_manejar_personaje(PACKED_ARGS){
	UNPACK_ARGS(tad_orquestador* self, tad_socket* socket);

	//establecemos la funcion manejadora de desconexion del socket
	SOCKET_ON_ERROR(socket, orquestador_cliente_desconectado(self, socket));

	//recibimos nombre y simbolo del personaje
	char* nombre = socket_receive_expected_string(socket, PERSONAJE_NOMBRE);
	char simbolo = socket_receive_expected_char(socket, PERSONAJE_SIMBOLO);
	logger_info(get_logger(self), "El cliente es el Personaje %s, con simbolo %c", nombre, simbolo);

	//recibimos su peticion de nivel o informe de objetivos completos
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 2, PERSONAJE_SOLICITUD_NIVEL, PERSONAJE_OBJETIVOS_COMPLETADOS);
	var(tipo, package_get_data_type(paquete));

	//desbindeamos el socket del multiplexor ya que esa fue la ultima comunicacion
	multiplexor_unbind_socket(self->multiplexor, socket);

	char* nombre_nivel;

	switch(tipo){
	case PERSONAJE_SOLICITUD_NIVEL:
		nombre_nivel = package_get_string(paquete);
		orquestador_personaje_solicita_nivel(self, socket, nombre, simbolo, nombre_nivel);
		break;
	case PERSONAJE_OBJETIVOS_COMPLETADOS:
		logger_info(get_logger(self), "El personaje %s informo que cumplio todos sus objetivos", nombre);
		free(nombre);
		socket_close(socket);
		un_personaje_termino_de_jugar(self->plataforma);
		break;
	}

	package_dispose(paquete);
}

void orquestador_personaje_solicita_nivel(tad_orquestador* orquestador, tad_socket* socket, char* nombre, char simbolo, char* nombre_nivel){
	logger_info(get_logger(orquestador), "El personaje %s solicito el nivel %s", nombre, nombre_nivel);

	//nos fijamos si el planificador del nivel que pidio se encuentra iniciado
	var(planificador, plataforma_planificador_iniciado(orquestador->plataforma, nombre_nivel));
	free(nombre_nivel);
	if(planificador){
		//derivamos el personaje al planificador
		planificador_agregar_personaje(planificador, nombre, simbolo, socket);
	}else{
		//pateamos al personaje
		logger_error(get_logger(orquestador), "El nivel que solicito el personaje %s no se encuentra iniciado", nombre);
		socket_close(socket);
	}
}
