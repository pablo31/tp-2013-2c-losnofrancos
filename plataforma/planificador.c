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

#define NOT_A_POSITION vector2_new(-1, -1)


/***************************************
 * DECLARACIONES ***********************
 ***************************************/

private void liberar_recursos_personaje(tad_personaje* personaje);

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

//manejo de desconexiones
private void nivel_desconectado(tad_planificador* self);
private void personaje_desconectado(tad_planificador* self, tad_personaje* personaje);


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
	var(s, self->semaforo);
	tad_personaje* ret = null;
	mutex_close(s);
	foreach(pj, lista, tad_personaje*)
		if(pj->simbolo == simbolo)
			ret = pj;
	mutex_open(s);
	return ret;
}

private void quitar_personaje(tad_planificador* self, tad_personaje* personaje, t_list* lista){
	bool personaje_buscado(tad_personaje* pj){
		return pj == personaje;
	}
	var(s, self->semaforo);
	mutex_close(s);
	list_remove_by_condition(lista, (void*)personaje_buscado);
	mutex_open(s);
}

private void agregar_personaje(tad_planificador* self, tad_personaje* personaje, t_list* lista){
	var(s, self->semaforo);
	mutex_close(s);
	list_add(lista, personaje);
	mutex_open(s);
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
	self->solicito_ubicacion_recurso = 0;
	//inicializamos el multiplexor y le bindeamos el socket del nivel
	var(multiplexor, multiplexor_create());
	multiplexor_bind_socket(multiplexor, socket_nivel, paquete_entrante_nivel, self);
	self->multiplexor = multiplexor;
	//inicializamos los semaforos
	self->semaforo = mutex_create();

	logger_info(get_logger(self), "Planificador del Nivel %s inicializado", nombre_nivel);
	return self;
}

/***************************************
 * MANEJO DE PERSONAJES ****************
 ***************************************/

void planificador_agregar_personaje(tad_planificador* self, char* nombre, char simbolo, tad_socket* socket){
	var(logger, get_logger(self));

	//alojamos una instancia de tad_personaje
	alloc(personaje, tad_personaje);
	personaje->nombre = nombre;
	personaje->simbolo = simbolo;
	personaje->socket = socket;
	//informamos al usuario
	logger_info(logger, "El personaje %s entro al nivel", nombre);

	SOCKET_ERROR_MANAGER(socket){
		socket_close(socket);
		liberar_recursos_personaje(personaje);
		return;
	}
	//nos presentamos
	socket_send_empty_package(socket, PRESENTACION_PLANIFICADOR);
	//recibimos la posicion inicial del personaje
	vector2 pos = socket_receive_expected_vector2(socket, PERSONAJE_POSICION);
	personaje->pos = pos;
	personaje->objetivo = NOT_A_POSITION;

	//buscamos algun personaje con el mismo simbolo
	var(listos, self->personajes_listos);
	tad_personaje* gemelo = buscar_personaje(self, simbolo, listos);
	if(gemelo != null){
		//ya hay otro personaje con el mismo simbolo
		logger_info(logger, "Ya existia un personaje con el mismo simbolo que %s", nombre);
		socket_close(socket);
		dealloc(personaje);
	}else{
		var(socket_nivel, self->nivel->socket);
		var(m, self->multiplexor);
		var(s, self->semaforo);

		SOCKET_ERROR_MANAGER(socket_nivel){
			mutex_open(s);
			nivel_desconectado(self);
			return;
		}

		mutex_close(s);

		//lo agregamos a la lista de personajes listos del planificador
		list_add(listos, personaje);
		//informamos al nivel y le pasamos los datos del personaje
		socket_send_empty_package(socket_nivel, PERSONAJE_CONECTADO);
		socket_send_char(socket_nivel, PERSONAJE_SIMBOLO, simbolo);
		socket_send_string(socket_nivel, PERSONAJE_NOMBRE, nombre);
		socket_send_vector2(socket_nivel, PERSONAJE_POSICION, pos);

		//bindeamos el socket al multiplexor
		multiplexor_bind_socket(m, socket, paquete_entrante_personaje, self, personaje);
		//desbloqueamos el multiplexor para que actualize su lista de fds
		multiplexor_simulate_io(m);

		mutex_open(s);
	}
}

private void liberar_recursos_personaje(tad_personaje* personaje){
	free(personaje->nombre);
	dealloc(personaje);
}

private void planificador_liberar_personaje(tad_planificador* self, tad_personaje* personaje){
	var(socket, personaje->socket);

	//cerramos su socket
	multiplexor_unbind_socket(self->multiplexor, socket);
	socket_close(socket);

	//lo quitamos de las listas
	if(self->personaje_actual == personaje) self->personaje_actual = null;
	quitar_personaje(self, personaje, self->personajes_listos);
	quitar_personaje(self, personaje, self->personajes_bloqueados);

	var(nombre, personaje->nombre);
	logger_info(get_logger(self), "El personaje %s fue pateado", nombre);

	liberar_recursos_personaje(personaje);
}

int planificador_esta_vacio(tad_planificador* self){
	var(s, self->semaforo);
	mutex_close(s);
	int listos = list_size(self->personajes_listos);
	int bloqueados = list_size(self->personajes_bloqueados);
	mutex_open(s);
	return !listos && !bloqueados;
}

/***************************************
 * FINALIZACION ************************
 ***************************************/

void planificador_finalizar(tad_planificador* self){
	multiplexor_dispose_and_dispose_objects(self->multiplexor);

	//liberamos los recursos de la lista de personajes listos
	foreach(personaje_listo, self->personajes_listos, tad_personaje*)
		liberar_recursos_personaje(personaje_listo);

	//liberamos los recursos de la lista de personajes bloqueados
	foreach(personaje_bloqueado, self->personajes_bloqueados, tad_personaje*)
		liberar_recursos_personaje(personaje_bloqueado);

	list_destroy(self->personajes_listos);
	list_destroy(self->personajes_bloqueados);

	//liberamos los recursos de los datos del nivel
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
		nivel_desconectado(self);
		return;
	}

	//el nivel nos indica la cantidad de quantums y el retardo entre turnos
	int quantum = socket_receive_expected_int(socket_nivel, QUANTUM);
	int retardo = socket_receive_expected_int(socket_nivel, RETARDO);
	char* algoritmo = socket_receive_expected_string(socket_nivel, ALGORITMO);
	logger_info(get_logger(self), "El quantum/distancia estimada sera de %d", quantum);
	logger_info(get_logger(self), "El retardo entre turnos sera de %dms", retardo);
	logger_info(get_logger(self), "El algoritmo de planificacion sera %s", algoritmo);
	self->quantum = quantum;
	self->retardo = retardo;
	if(string_equals("SRDF", algoritmo))
		self->algoritmo = algoritmo_srdf;
	else
		self->algoritmo = algoritmo_rr;
	free(algoritmo);

	while(1){
		multiplexor_wait_for_io(self->multiplexor);
		if(!self->personaje_actual) otorgar_turno(self);
	}
}

private void paquete_entrante_nivel(PACKED_ARGS){
	UNPACK_ARG(tad_planificador* self);

	var(socket_nivel, self->nivel->socket);
	SOCKET_ERROR_MANAGER(socket_nivel){
		nivel_desconectado(self);
		return;
	}

	tad_package* paquete = socket_receive_one_of_this_packages(socket_nivel, 7,
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
		logger_info(logger, "La cantidad del quantum/distancia estimada cambio a %d", quantum);
		self->quantum = quantum;

	}else if(tipo == RETARDO){
		int retardo = package_get_int(paquete);
		logger_info(logger, "El retardo entre turnos cambio a %dms", retardo);
		self->retardo = retardo;

	}else if (tipo == ALGORITMO){
		char* algoritmo = package_get_string(paquete);
		logger_info(logger, "El algoritmo de planificacion cambio a %s", algoritmo);

		if(string_equals("SRDF", algoritmo))
			self->algoritmo = algoritmo_srdf;
		else
			self->algoritmo = algoritmo_rr;

	}else if(tipo == UBICACION_RECURSO){
		var(personaje, self->personaje_actual);
		if(personaje != null && self->solicito_ubicacion_recurso){
			var(socket, personaje->socket);
			vector2 objetivo = package_get_vector2(paquete);
			personaje->objetivo = objetivo;
			socket_send_package(socket, paquete);
			socket_send_empty_package(socket, PLANIFICADOR_OTORGA_TURNO);
		}

	}else if(tipo == RECURSO_OTORGADO){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_bloqueados));
		quitar_personaje(self, personaje, self->personajes_bloqueados);
		personaje->objetivo = NOT_A_POSITION;
		logger_info(logger, "El recurso que solicito %s le fue otorgado", personaje->nombre);
		agregar_personaje(self, personaje, self->personajes_listos);
		socket_send_empty_package(personaje->socket, RECURSO_OTORGADO);

	}else if(tipo == MUERTE_POR_ENEMIGO){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_listos));
		logger_info(logger, "El personaje %s muere por un enemigo", personaje->nombre);
		socket_send_empty_package(personaje->socket, MUERTE_POR_ENEMIGO);
		multiplexor_stop_io_handling(self->multiplexor);
		planificador_liberar_personaje(self, personaje);

	}else if(tipo == MUERTE_POR_DEADLOCK){
		var(simbolo, package_get_char(paquete));
		var(personaje, buscar_personaje(self, simbolo, self->personajes_bloqueados));
		logger_info(logger, "El personaje %s muere por algoritmo deadlock", personaje->nombre);
		socket_send_empty_package(personaje->socket, MUERTE_POR_DEADLOCK);
		multiplexor_stop_io_handling(self->multiplexor);
		planificador_liberar_personaje(self, personaje);

	}

	package_dispose_all(paquete);
}

private void paquete_entrante_personaje(PACKED_ARGS){
	UNPACK_ARGS(tad_planificador* self, tad_personaje* personaje);
	var(socket_nivel, self->nivel->socket);
	var(socket, personaje->socket);
	var(logger, get_logger(self));

	var(nombre, personaje->nombre);
	var(simbolo, personaje->simbolo);

	SOCKET_ERROR_MANAGER(socket){
		personaje_desconectado(self, personaje);
		return;
	}

	//si no es el personaje que tiene el turno actualmente, lo pateamos
	if(personaje != self->personaje_actual){
		socket_set_error(socket, UNEXPECTED_PACKAGE);
		return;
	}

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
		self->solicito_ubicacion_recurso = 1;
		socket_send_package(socket_nivel, paquete);

	//el personaje avisa que va a realizar un movimiento
	}else if(tipo_mensaje == PERSONAJE_MOVIMIENTO){
		vector2 pos = package_get_vector2(paquete);
		logger_info(logger, "%s se mueve a (%d,%d)", nombre, pos.x, pos.y);
		personaje->pos = pos;
		tad_package* reenvio = package_create_char_and_vector2(PERSONAJE_MOVIMIENTO, simbolo, pos);
		socket_send_package(socket_nivel, reenvio);
		package_dispose_all(reenvio);
		self->turnos_restantes--;
		if(self->turnos_restantes)
			socket_send_empty_package(socket, PLANIFICADOR_OTORGA_TURNO);
		else
			self->personaje_actual = null;
		usleep(self->retardo * 1000);

	//el personaje solicita una instancia de un recurso
	}else if(tipo_mensaje == PERSONAJE_SOLICITUD_RECURSO){
		char recurso = package_get_char(paquete);
		logger_info(logger, "%s solicito una instancia del recurso %c", nombre, recurso);
		tad_package* reenvio = package_create_two_chars(PERSONAJE_SOLICITUD_RECURSO, simbolo, recurso);
		socket_send_package(socket_nivel, reenvio);
		package_dispose_all(reenvio);
		self->personaje_actual = null;
		self->turnos_restantes = 0;
		quitar_personaje(self, personaje, self->personajes_listos);
		list_add(self->personajes_bloqueados, personaje);

	}

	package_dispose_all(paquete);
}

private void otorgar_turno(tad_planificador* self){
	//informamos el estado de las colas
	var(listos, self->personajes_listos);
	var(bloqueados, self->personajes_bloqueados);

	var(s, self->semaforo);

	mutex_close(s);

	if(list_size(listos) > 0) mostrar_lista(self, "Cola de listos", listos);
	if(list_size(bloqueados) > 0) mostrar_lista(self, "Cola de bloqueados", bloqueados);

	//obtenemos el siguiente personaje al que le toca jugar
	var(personaje, self->algoritmo(self));
	self->personaje_actual = personaje;
	self->solicito_ubicacion_recurso = 0;

	mutex_open(s);

	if(!personaje) return;
	if(personaje) logger_info(get_logger(self), "El siguiente en jugar sera %s (%c)", personaje->nombre, personaje->simbolo);

	//TODO error manager?
	socket_send_empty_package(personaje->socket, PLANIFICADOR_OTORGA_TURNO);
}

private tad_personaje* algoritmo_rr(tad_planificador* self){
	var(personajes, self->personajes_listos);

	//verificamos que haya personajes
	if(list_size(personajes) == 0) return null;

	//movemos el primero de la lista al final
	tad_personaje* pj = list_remove(personajes, 0);
	list_add(personajes, pj);
	self->turnos_restantes = self->quantum;

	//devolvemos el primero de la lista
	return list_get(personajes, 0);
}

private int algoritmo_srdf_distancia(tad_planificador* self, tad_personaje* personaje){
	int distancia_profetizada = self->quantum;

	if(vector2_equals(personaje->objetivo, NOT_A_POSITION))
		return distancia_profetizada;
	else
		return vector2_distance_to(personaje->pos, personaje->objetivo);
}

private tad_personaje* algoritmo_srdf(tad_planificador* self){
	var(personajes, self->personajes_listos);

	//verificamos que haya personajes
	if(list_size(personajes) == 0) return null;

	tad_personaje* sdr_pj = list_get(personajes, 0);
	int sdr = algoritmo_srdf_distancia(self, sdr_pj);

	foreach(personaje, personajes, tad_personaje*){
		int distance = algoritmo_srdf_distancia(self, personaje);
		if(distance < sdr){
			sdr_pj = personaje;
			sdr = distance;
		}
	}

	self->turnos_restantes = sdr;
	return sdr_pj;
}




private void mostrar_lista(tad_planificador* self, char* header, t_list* personajes){
	char str[256];
	int i = 0;

	foreach(personaje, personajes, tad_personaje*){
		str[i] = personaje->simbolo;
		i++;
	}
	str[i] = '\0';

	logger_info(get_logger(self), "%s: %s", header, str);
}





/***************************************
 * MANEJO DE DESCONEXIONES *************
 ***************************************/

private void nivel_desconectado(tad_planificador* self){
	logger_info(get_logger(self), "El nivel se desconecto inesperadamente");
	plataforma_finalizar_planificador(self->plataforma, self);
}

private void personaje_desconectado(tad_planificador* self, tad_personaje* personaje){
	var(socket_nivel, self->nivel->socket);

	SOCKET_ERROR_MANAGER(socket_nivel){
		nivel_desconectado(self);
		return;
	}

	logger_info(get_logger(self), "El personaje %s se desconecto de manera inesperada", personaje->nombre);
	socket_send_char(socket_nivel, PERSONAJE_DESCONEXION, personaje->simbolo);
	planificador_liberar_personaje(self, personaje);
}
