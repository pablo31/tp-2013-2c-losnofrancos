/*
 * planificador.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <string.h>

#include "../libs/common/string.h"

#include "../libs/multiplexor/multiplexor.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/socket/package_serializers.h"
#include "../libs/protocol/protocol.h"
#include "../libs/vector/vector2.h"

#include "planificador.h"


/***************************************
 * DECLARACIONES ***********************
 ***************************************/

//interaccion con nivel
private void paquete_entrante_nivel(PACKED_ARGS);
private void paquete_entrante_personaje(PACKED_ARGS);

//interaccion con personaje
private void otorgar_turno(tad_planificador* self);

//algoritmo planificador
private tad_personaje* algoritmo_srdf(tad_planificador* self);
private tad_personaje* algoritmo_rr(tad_planificador* self);

//busca a un personaje
private tad_personaje* buscar_personaje(tad_planificador* self, char simbolo, t_list* lista);

//logeo
private void mostrar_lista(tad_planificador* self, char* header, t_list* personajes);


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




private tad_personaje* buscar_personaje(tad_planificador* self, char simbolo, t_list* lista){
	foreach(pj, lista, tad_personaje*)
		if(pj->simbolo == simbolo)
			return pj;
	return null;
}

private void quitar_personaje(tad_planificador* self, tad_personaje* personaje, t_list* lista){
	bool personaje_buscado(tad_personaje* pj){
		return pj == personaje;
	}
	list_remove_by_condition(lista, (void*)personaje_buscado);
}


/***************************************
 * CREACION ****************************
 ***************************************/

tad_planificador* planificador_crear(char* nombre_nivel, tad_socket* socket_nivel, tad_plataforma* plataforma){
	//alojamos una estructura tad_planificador
	alloc(self, tad_planificador);
	self->plataforma = plataforma;
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
	self->personaje_actual = null;
	self->turnos_restantes = 0;
	self->bandera = 0;
	//inicializamos el multiplexor del nivel y le bindeamos el socket
	var(mpx_nivel, multiplexor_create());
	multiplexor_bind_socket(mpx_nivel, socket_nivel, paquete_entrante_nivel, self);
	self->mpx_nivel = mpx_nivel;
	//inicializamos el multiplexor de personajes
	self->mpx_personajes = multiplexor_create();
	//inicializamos los semaforos
	self->semaforo = mutex_create();

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
	//informamos al usuario
	logger_info(get_logger(self), "El personaje %s entro al nivel", nombre);
	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PLANIFICADOR);
	//recibimos la posicion inicial del personaje
	vector2 pos = socket_receive_expected_vector2(socket, PERSONAJE_POSICION);

	self->bandera = 1;
	var(sem, self->semaforo);
	mutex_close(sem);

	//lo agregamos a la lista de personajes listos del planificador
	list_add(self->personajes_listos, personaje);
	//bindeamos el socket al multiplexor
	multiplexor_bind_socket(self->mpx_personajes, socket, paquete_entrante_personaje, self, personaje);

	//informamos al nivel y le pasamos los datos del personaje
	var(sn, self->nivel->socket);
	socket_send_empty_package(sn, PERSONAJE_CONECTADO);
	socket_send_char(sn, PERSONAJE_SIMBOLO, simbolo);
	socket_send_string(sn, PERSONAJE_NOMBRE, nombre);
	socket_send_vector2(sn, PERSONAJE_POSICION, pos);

	self->bandera = 0;
	mutex_open(sem);
}

private void planificador_liberar_personaje(tad_planificador* self, tad_personaje* personaje){
	//informamos al nivel
	socket_send_char(self->nivel->socket, PERSONAJE_DESCONEXION, personaje->simbolo);

	var(socket, personaje->socket);

	//cerramos su socket
	multiplexor_unbind_socket(self->mpx_personajes, socket);
	socket_close(socket);

	//lo quitamos de las listas
	if(self->personaje_actual == personaje) self->personaje_actual = null;
	quitar_personaje(self, personaje, self->personajes_listos);
	quitar_personaje(self, personaje, self->personajes_bloqueados);

	var(nombre, personaje->nombre);
	logger_info(get_logger(self), "El personaje %s fue pateado", nombre);

	free(nombre);
	dealloc(personaje);
}

/***************************************
 * FINALIZACION ************************
 ***************************************/

void planificador_finalizar(tad_planificador* self){
	//liberamos los recursos de la lista de personajes listos
	foreach(personaje_listo, self->personajes_listos, tad_personaje*)
		planificador_liberar_personaje(self, personaje_listo);
	list_destroy(self->personajes_listos);

	//liberamos los recursos de la lista de personajes bloqueados
	foreach(personaje_bloqueado, self->personajes_bloqueados, tad_personaje*)
		planificador_liberar_personaje(self, personaje_bloqueado);
	list_destroy(self->personajes_bloqueados);

	//liberamos los recursos del multiplexor de personajes
	multiplexor_dispose(self->mpx_personajes);

	//liberamos los recursos de los datos del nivel
	multiplexor_dispose_and_dispose_objects(self->mpx_nivel);
	var(nivel, self->nivel);
	free(nivel->nombre);
	dealloc(nivel);

	//liberamos los semaforos
	mutex_dispose(self->semaforo);

	//liberamos los recursos propios del planificador
	logger_info(get_logger(self), "Finalizado");
	logger_dispose_instance(self->logger);
	dealloc(self);
}





/***************************************
 * LOGICA ******************************
 ***************************************/


void planificador_ejecutar(PACKED_ARGS){
	UNPACK_ARG(tad_planificador* self);

	//seteamos el manejo de errores ante una desconexion del nivel
	var(socket_nivel, self->nivel->socket);
	SOCKET_ERROR_MANAGER(socket_nivel){
		logger_info(get_logger(self), "El nivel se desconecto inesperadamente");
		plataforma_finalizar_planificador(self->plataforma, self);
		return;
	}

	//el nivel nos indica la cantidad de quantums y el retardo entre turnos
	int quantum = socket_receive_expected_int(socket_nivel, QUANTUM);
	int retardo = socket_receive_expected_int(socket_nivel, RETARDO);
	char* algoritmo = socket_receive_expected_string(socket_nivel, ALGORITMO);
	logger_info(get_logger(self), "La cantidad de quantums sera de %d", quantum);
	logger_info(get_logger(self), "El retardo entre cambio de turno sera de %dms", retardo);
	logger_info(get_logger(self), "El algoritmo de planificacion sera %s", algoritmo);
	self->quantum = quantum;
	self->retardo = retardo;
	if(string_equals("SRDF", algoritmo))
		self->algoritmo = algoritmo_srdf;
	else
		self->algoritmo = algoritmo_rr;
	free(algoritmo);

	var(sem, self->semaforo);
	mutex_close(sem);

	while(1){
		if(self->bandera){
			mutex_open(sem);
			usleep(500 * 1000); //dejamos medio segundo para forzar a que entren los personajes
			mutex_close(sem);
		}
		//primero atendemos los paquetes del nivel, por si se quitan personajes de las listas
		multiplexor_wait_for_io(self->mpx_nivel, 1);
		//si no hay personaje jugando, otorgamos un turno (si podemos)
		if(!self->personaje_actual) otorgar_turno(self);
		//luego atendemos a los personajes restantes
		multiplexor_wait_for_io(self->mpx_personajes, 1);
	}
}









private void paquete_entrante_nivel(PACKED_ARGS){
	UNPACK_ARG(tad_planificador* self);

	tad_package* paquete = socket_receive_one_of_this_packages(self->nivel->socket, 7,
			//interaccion con los personajes
			RECURSO_OTORGADO,
			UBICACION_RECURSO,
			//cambios en la configuracion
			QUANTUM,
			RETARDO,
			ALGORITMO,
			//muertes
			MUERTE_POR_DEADLOCK,
			MUERTE_POR_ENEMIGO);

	var(tipo, package_get_data_type(paquete));
	var(logger, get_logger(self));

	if(tipo == QUANTUM){
		int quantum = package_get_int(paquete);
		logger_info(logger, "La cantidad del quantum cambio a %d", quantum);
		self->quantum = quantum;

	}else if(tipo == RETARDO){
		int retardo = package_get_int(paquete);
		logger_info(logger, "El retardo entre cambio de turno cambio a %dms", retardo);
		self->retardo = retardo;

	}else if (tipo == ALGORITMO){
		char* algoritmo = package_get_string(paquete);
		logger_info(logger, "El algoritmo de planificacion cambio a %s", algoritmo);

		if(string_equals("SRDF", algoritmo))
			self->algoritmo = algoritmo_srdf;
		else
			self->algoritmo = algoritmo_rr;
		free(algoritmo);

	}else if(tipo == RECURSO_OTORGADO){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_bloqueados));
		logger_info(logger, "El recurso que solicito %s le fue otorgado", personaje->nombre);
		list_add(self->personajes_listos, personaje);
		socket_send_empty_package(personaje->socket, RECURSO_OTORGADO);

	}else if(tipo == MUERTE_POR_ENEMIGO){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_listos));
		logger_info(logger, "El personaje %s muere por un enemigo", personaje->nombre);
		socket_send_empty_package(personaje->socket, MUERTE_POR_ENEMIGO);
		planificador_liberar_personaje(self, personaje);

	}else if(tipo == MUERTE_POR_DEADLOCK){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_bloqueados));
		logger_info(logger, "El personaje %s muere por algoritmo deadlock", personaje->nombre);
		socket_send_empty_package(personaje->socket, MUERTE_POR_DEADLOCK);
		planificador_liberar_personaje(self, personaje);

	}else if(tipo == UBICACION_RECURSO){
		var(socket, self->personaje_actual->socket);
		socket_send_package(socket, paquete);
		socket_send_empty_package(socket, PLANIFICADOR_OTORGA_TURNO);

	}

	package_dispose(paquete);
}



private void paquete_entrante_personaje(PACKED_ARGS){
	UNPACK_ARGS(tad_planificador* self, tad_personaje* personaje);
	var(socket, personaje->socket);

	SOCKET_ERROR_MANAGER(socket){
		logger_info(get_logger(self), "El personaje %s se desconecto de manera inesperada", personaje->nombre);
		planificador_liberar_personaje(self, personaje);
		return;
	}

	//si no es el personaje que tiene el turno actualmente, lo pateamos
	if(personaje != self->personaje_actual){
		socket_set_error(socket, UNEXPECTED_PACKAGE);
		return; //decoroso return que no hace falta
	}

	var(nombre, personaje->nombre);
	var(simbolo, personaje->simbolo);

	var(logger, get_logger(self));
	var(socket_nivel, self->nivel->socket);

	//recibimos el paquete
	tad_package* paquete = socket_receive_one_of_this_packages(socket, 3,
					SOLICITUD_UBICACION_RECURSO,
					PERSONAJE_MOVIMIENTO,
					PERSONAJE_SOLICITUD_RECURSO);
	var(tipo_mensaje, package_get_data_type(paquete));

	//el personaje solicita la posicion de un recurso
	if(tipo_mensaje == SOLICITUD_UBICACION_RECURSO){
		char recurso = package_get_char(paquete);
		logger_info(logger, "%s solicito la ubicacion del recurso %c", nombre, recurso);
		socket_send_package(socket_nivel, paquete);

	//el personaje avisa que va a realizar un movimiento
	}else if(tipo_mensaje == PERSONAJE_MOVIMIENTO){
		vector2 direccion = package_get_vector2(paquete);
		logger_info(logger, "%s se mueve a (%d,%d)", nombre, direccion.x, direccion.y);
		tad_package* reenvio = package_create_char_and_vector2(PERSONAJE_MOVIMIENTO, simbolo, direccion);
		socket_send_package(socket_nivel, reenvio);
		self->turnos_restantes--;
		if(self->turnos_restantes)
			socket_send_empty_package(socket, PLANIFICADOR_OTORGA_TURNO);
		else
			self->personaje_actual = null;

	//el personaje solicita una instancia de un recurso
	}else if(tipo_mensaje == PERSONAJE_SOLICITUD_RECURSO){
		char recurso = package_get_char(paquete);
		logger_info(logger, "%s solicito una instancia del recurso %c", nombre, recurso);
		tad_package* reenvio = package_create_two_chars(PERSONAJE_SOLICITUD_RECURSO, simbolo, recurso);
		socket_send_package(socket_nivel, reenvio);
		self->personaje_actual = null;
		self->turnos_restantes = 0;
		quitar_personaje(self, personaje, self->personajes_listos);
		list_add(self->personajes_bloqueados, personaje);
	}

	package_dispose(paquete);
}







private void otorgar_turno(tad_planificador* self){
	//informamos el estado de las colas
	var(listos, self->personajes_listos);
	var(bloqueados, self->personajes_bloqueados);
	if(list_size(listos) > 0) mostrar_lista(self, "Cola de listos", listos);
	if(list_size(bloqueados) > 0) mostrar_lista(self, "Cola de bloqueados", bloqueados);

	//obtenemos el siguiente personaje al que le toca jugar
	var(personaje, self->algoritmo(self));
	self->personaje_actual = personaje;

	if(!personaje) return;

	self->turnos_restantes = self->quantum;
	if(personaje) logger_info(get_logger(self), "El siguiente en jugar sera %s (%c)", personaje->nombre, personaje->simbolo);

	usleep(self->retardo * 1000);

	socket_send_empty_package(personaje->socket, PLANIFICADOR_OTORGA_TURNO);
}





private tad_personaje* algoritmo_rr(tad_planificador* self){
	var(personajes, self->personajes_listos);

	//verificamos que haya personajes
	if(list_size(personajes) == 0) return null;

	//movemos el primero de la lista al final
	tad_personaje* pj = list_remove(personajes, 0);
	list_add(personajes, pj);

	//devolvemos el primero de la lista
	return list_get(personajes, 0);
}

private tad_personaje* algoritmo_srdf(tad_planificador* self){
	//TODO logica de srdf (temporalmente devuelve siempre el 1er personaje de la lista)
	var(personajes, self->personajes_listos);
	if(list_size(personajes) > 0) return list_get(personajes, 0);
	else return null;
}










private void mostrar_lista(tad_planificador* self, char* header, t_list* personajes){
/*	char* s = "";
	foreach(personaje, personajes, tad_personaje*){
		char* tmp;
		tmp = string_from_format("%s%c", s, personaje->simbolo);
		if(s!=null) free(s);
		s = tmp;
	}
	logger_info(get_logger(self), "%s: %s", header, s);
	free(s);
	*/
}


