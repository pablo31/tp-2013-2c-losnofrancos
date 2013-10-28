/*
 * planificador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include "../libs/multiplexor/multiplexor.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/socket/package_serializers.h"
#include "../libs/common.h"
#include "../libs/protocol/protocol.h"
#include "../libs/vector/vector2.h"

#include "planificador.h"


/***************************************
 * DECLARACIONES ***********************
 ***************************************/

//interaccion con nivel
private void paquete_entrante_nivel(PACKED_ARGS);
private void manejar_paquete_nivel(tad_planificador* self, tad_package* paquete);

//interaccion con personaje
private void otorgar_turno(tad_planificador* self, int quantum);
private tad_package* esperar_ubicacion_recurso(tad_planificador* self, tad_socket* socket_nivel);

//algoritmo planificador
private tad_personaje* siguiente_personaje(tad_planificador* self);

//bloqueo y desbloqueo de personajes
private void bloquear_personaje(tad_planificador* self, tad_personaje* personaje);
private tad_personaje* buscar_personaje_bloqueado(tad_planificador* self, char simbolo);

//manejo de desconexiones
private void error_socket_personaje(tad_planificador* self, tad_personaje* personaje);
private void error_socket_nivel(tad_planificador* self);


/***************************************
 * GETTERS *****************************
 ***************************************/

private char* get_nombre_nivel(tad_planificador* self){
	return self->nivel->nombre;
}
private tad_logger* get_logger(tad_planificador* self){
	return self->logger;
}
char* planificador_nombre_nivel(tad_planificador* self){
	return get_nombre_nivel(self);
}



/***************************************
 * CREACION ****************************
 ***************************************/

tad_planificador* planificador_crear(char* nombre_nivel, tad_socket* socket_nivel){
	//alojamos una estructura tad_planificador
	alloc(self, tad_planificador);
	//obtenemos una instancia del logger
	self->logger = logger_new_instance("Planificador %s", nombre_nivel);
	//guardamos los datos del nivel
	alloc(nivel, tad_nivel);
	nivel->nombre = nombre_nivel;
	nivel->socket = socket_nivel;
	self->nivel = nivel;
	//inicializamos las colas de personajes
	self->personajes_listos = list_create();
	self->personajes_bloqueados = list_create();
	//inicializamos el multiplexor y le bindeamos el socket del nivel
	var(m, multiplexor_create());
	multiplexor_bind_socket(m, socket_nivel, paquete_entrante_nivel, 1, self);
	self->multiplexor = m;

	logger_info(get_logger(self), "Planificador del Nivel %s inicializado", nombre_nivel);
	return self;
}

/***************************************
 * MANEJO DE PERSONAJES ****************
 ***************************************/

void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket){
	//alojamos una instancia de tad_personaje
	alloc(personaje, tad_personaje);
	personaje->nombre = nombre;
	personaje->simbolo = simbolo;
	personaje->socket = socket;
	//lo agregamos a la lista de personajes listos del planificador
	list_add(self->personajes_listos, personaje);
	//informamos al usuario
	logger_info(get_logger(self), "El personaje %s entro al nivel", nombre);
	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PLANIFICADOR);
}

private void planificador_liberar_personaje(tad_planificador* self, tad_personaje* personaje){
	socket_close(personaje->socket);
	var(nombre, personaje->nombre);
	logger_info(get_logger(self), "El personaje %s fue pateado", nombre);
	free(nombre);
	dealloc(personaje);
}

/***************************************
 * FINALIZACION ************************
 ***************************************/

void planificador_finalizar(tad_planificador* self){
	logger_info(get_logger(self), "Finalizando");

	//liberamos los recursos de los datos de los personajes
	void destroyer(void* ptr_personaje){
		planificador_liberar_personaje(self, ptr_personaje);
	}
	list_destroy_and_destroy_elements(self->personajes_listos, destroyer);
	list_destroy_and_destroy_elements(self->personajes_bloqueados, destroyer);

	//liberamos los recursos de los datos del nivel
	var(m, self->multiplexor);
	var(nivel, self->nivel);
	var(socket_nivel, nivel->socket);
	multiplexor_unbind_socket(m, socket_nivel);
	socket_close(socket_nivel);
	dealloc(nivel);

	//liberamos los recursos del multiplexor
	multiplexor_dispose(m);

	//liberamos los recursos propios del planificador
	logger_dispose_instance(self->logger);
	free(self->algoritmo);
	dealloc(self);
}





/***************************************
 * LOGICA ******************************
 ***************************************/


void planificador_ejecutar(PACKED_ARGS){
	UNPACK_ARG(tad_planificador* self);

	//seteamos el manejo de errores ante una desconexion del nivel
	var(socket_nivel, self->nivel->socket);
	SOCKET_ON_ERROR(socket_nivel, error_socket_nivel(self));

	//el nivel nos indica la cantidad de quantums y el retardo entre turnos
	int quantum = socket_receive_expected_int(socket_nivel, QUANTUM);
	int retardo = socket_receive_expected_int(socket_nivel, RETARDO);
	char* algoritmo = socket_receive_expected_string(socket_nivel, ALGORITMO);
	logger_info(get_logger(self), "La cantidad de quantums sera de %d", quantum);
	logger_info(get_logger(self), "El retardo entre cambio de turno sera de %dms", retardo);
	self->quantum = quantum;
	self->retardo = retardo;
	self->algoritmo = algoritmo;

	int retardo_faltante;

	while(1){
		//aprovechamos el tiempo de retardo para ejecutar un select sobre el socket del nivel
		multiplexor_wait_for_io(self->multiplexor, self->retardo, out retardo_faltante);
		if(retardo_faltante > 0) usleep(retardo_faltante * 1000);
		//ejecutamos la logica
		otorgar_turno(self, self->quantum);
	}
}

private tad_package* esperar_ubicacion_recurso(tad_planificador* self, tad_socket* socket_nivel){
	tad_package* paquete;

	while(1){
		paquete = socket_receive_one_of_this_packages(socket_nivel, 4,
				UBICACION_RECURSO, //este es el unico que nos interesa de verdad
				RECURSO_OTORGADO,
				QUANTUM,
				RETARDO);
		var(tipo, package_get_data_type(paquete));

		if(tipo == UBICACION_RECURSO) break;
		else manejar_paquete_nivel(self, paquete);

		package_dispose(paquete);
	}

	return paquete;
}

private void otorgar_turno(tad_planificador* self, int quantum){
	//obtenemos el siguiente personaje al que le toca jugar
	var(personaje, siguiente_personaje(self));
	if(!personaje) return;

	var(simbolo, personaje->simbolo);
	var(socket, personaje->socket);
	var(socket_nivel, self->nivel->socket);

	//seteamos el manejo de errores ante una desconexion del personaje
	SOCKET_ON_ERROR(socket, error_socket_personaje(self, personaje));

	while(quantum--){
		socket_send_empty_package(socket, PLANIFICADOR_OTORGA_QUANTUM);

		tad_package* paquete = socket_receive_one_of_this_packages(socket, 3,
				SOLICITUD_UBICACION_RECURSO,
				PERSONAJE_MOVIMIENTO,
				PERSONAJE_SOLICITUD_RECURSO);
		var(tipo_mensaje, package_get_data_type(paquete));

		//el personaje solicita la posicion de un recurso
		if(tipo_mensaje == SOLICITUD_UBICACION_RECURSO){
			//reenviamos la solicitud al nivel
			socket_send_package(socket_nivel, paquete);
			//esperamos su respuesta
			tad_package* respuesta = esperar_ubicacion_recurso(self, socket_nivel);
			//se la reenviamos al personaje
			socket_send_package(socket, respuesta);
			package_dispose(respuesta);

		//el personaje avisa que va a realizar un movimiento
		}else if(tipo_mensaje == PERSONAJE_MOVIMIENTO){
			vector2 direccion = package_get_vector2(paquete);
			tad_package* reenvio = package_create_char_and_vector2(PERSONAJE_MOVIMIENTO, simbolo, direccion);
			socket_send_package(socket_nivel, reenvio);
			package_dispose(reenvio);

		//el personaje solicita una instancia de un recurso
		}else if(tipo_mensaje == PERSONAJE_SOLICITUD_RECURSO){
			char recurso = package_get_char(paquete);
			tad_package* reenvio = package_create_two_chars(PERSONAJE_SOLICITUD_RECURSO, simbolo, recurso);
			socket_send_package(socket_nivel, reenvio);
			package_dispose(reenvio);
			bloquear_personaje(self, personaje);
		}

		package_dispose(paquete);
	}
}

private tad_personaje* siguiente_personaje(tad_planificador* self){
	var(personajes, self->personajes_listos);
	if(list_size(personajes) > 0) return list_remove(personajes, 0);
	else return null;
	//TODO algoritmo intercambiable, etc
}


private void bloquear_personaje(tad_planificador* self, tad_personaje* personaje){
	bool personaje_buscado(void* ptr_pj){
		return ptr_pj == personaje;
	}
	list_remove_by_condition(self->personajes_listos, personaje_buscado);
	list_add(self->personajes_bloqueados, personaje);
}

private tad_personaje* buscar_personaje_bloqueado(tad_planificador* self, char simbolo){
	bool personaje_buscado(void* personaje){
		return ((tad_personaje*)personaje)->simbolo == simbolo;
	}
	return list_remove_by_condition(self->personajes_bloqueados, personaje_buscado);
}




private void manejar_paquete_nivel(tad_planificador* self, tad_package* paquete){
	var(tipo, package_get_data_type(paquete));
	if(tipo == QUANTUM){
		self->quantum = package_get_int(paquete);
	}else if(tipo == RETARDO){
		self->retardo = package_get_int(paquete);
	}else if(tipo == RECURSO_OTORGADO){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje_bloqueado(self, simbolo));
		list_add(self->personajes_listos, personaje);
		socket_send_empty_package(personaje->socket, RECURSO_OTORGADO);
	}
}


private void paquete_entrante_nivel(PACKED_ARGS){
	UNPACK_ARG(tad_planificador* self);

	tad_package* paquete = socket_receive_one_of_this_packages(self->nivel->socket, 3,
			RECURSO_OTORGADO,
			QUANTUM,
			RETARDO);

	manejar_paquete_nivel(self, paquete);
	package_dispose(paquete);
}










private void error_socket_personaje(tad_planificador* self, tad_personaje* personaje){
	logger_info(get_logger(self), "El personaje %s se desconecto de manera inesperada", personaje->nombre);
	planificador_liberar_personaje(self, personaje);
}

private void error_socket_nivel(tad_planificador* self){
	logger_info(get_logger(self), "El nivel asociado al planificador se desconecto inesperadamente");
	planificador_finalizar(self);
}











