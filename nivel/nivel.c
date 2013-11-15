#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../libs/socket/socket_utils.h"
#include "../libs/socket/package_serializers.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/notifier/notifier.h"
#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/protocol/protocol.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"
#include "enemigo.h"
#include "verificador_deadlock.h"

private void nivel_conectar_a_plataforma(tad_nivel* self, char* ippuerto);
private void nivel_iniciar_interfaz_grafica(tad_nivel* self);
private void nivel_move_enemigos(tad_nivel* self);
private void nuevo_hilo_enemigo(PACKED_ARGS);
//private void algotirmo_vereficador_deadlock_activate(tad_nivel* self);
private void nivel_ejecutar_logica(tad_nivel* self);


private void manejar_desconexion(tad_nivel* self);
private void manejar_desconexion_multiplexor(tad_nivel* self, tad_multiplexor* m);
private void manejar_paquete_planificador(PACKED_ARGS);
private void config_file_modified(PACKED_ARGS);

private void nivel_finalizar(tad_nivel* self);
private void nivel_finalizar_cerrar_multiplexor(tad_nivel* self, tad_multiplexor* m);



/***************************************************************
 * Misc
 ***************************************************************/

private void verificar_argumentos(int argc, char* argv[]) {
	if(argc > 1) return;
	printf("Error: Debe ingresar el nombre del archivo de configuracion\n");
	exit(EXIT_FAILURE);
}

/***************************************************************
 * Getters
 ***************************************************************/

tad_logger* get_logger(tad_nivel* self){
	return self->logger;
}

char* get_config_path(tad_nivel* self){
	return self->config_path;
}



/***************************************************************
 * MAIN
 ***************************************************************/

int main(int argc, char **argv){

	verificar_argumentos(argc, argv);
	char* config_path = argv[1];

	//gui_item* items;

	srand(time(NULL)); //seed para random

	//inicializo el nivel
	char* ippuerto;
	tad_nivel* self = crear_nivel(config_path, out ippuerto);

	//declaro la funcion manejadora de sigint
	signal_dynamic_handler(SIGINT, nivel_finalizar(self));

	//nos conectamos al planificador
	nivel_conectar_a_plataforma(self, ippuerto);
	free(ippuerto);

	//iniciamos la gui
	nivel_iniciar_interfaz_grafica(self);

	//se mueven los enemigos
	nivel_move_enemigos(self);

	//algoritmo vereficador de deadlock
	//verificador_deadlock(self, items);

	//ejecutamos la logica
	nivel_ejecutar_logica(self);


	//no deberia llegar hasta aca, pero por si las dudas...
	nivel_finalizar(self);
	return EXIT_FAILURE;
}


/***************************************************************
 * Ejecucion y logica
 ***************************************************************/

private void nivel_conectar_a_plataforma(tad_nivel* self, char* ippuerto){
	var(logger, get_logger(self));

	//Me conecto con el orquestador
	var(ip, string_get_ip(ippuerto));
	var(puerto, string_get_port(ippuerto));
	logger_info(logger, "Conectando a %s", ippuerto);

	tad_socket* socket = socket_connect(ip, puerto);
	logger_info(logger, "Conectado con Plataforma");
	self->socket = socket;

	//Si el socket pierde la conexion...
	SOCKET_ON_ERROR(socket, manejar_desconexion(self));

	//Esperamos la presentacion del orquestador
	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);
	logger_info(logger, "El servidor es un Orquestador");

	//Nos presentamos
	socket_send_empty_package(socket, PRESENTACION_NIVEL);

	//Indicamos nuestro nombre
	socket_send_string(socket, NIVEL_NUMERO, self->nombre);
	//Le indicamos la cantidad de quantums al planificador
	socket_send_int(socket, QUANTUM, self->quantum);
	//Le indicamos la pausa entre turnos al planificador
	socket_send_int(socket, RETARDO, self->retardo);
	//Le indicamos el algoritmo al planificador
	socket_send_string(socket, ALGORITMO, self->algoritmo);
}


private void nivel_iniciar_interfaz_grafica(tad_nivel* self){
	logger_info(get_logger(self), "Inicializando interfaz grafica");
	//Intento iniciar la GUI
	nivel_gui_inicializar();
	//Cargo los recursos en la pantalla
	cargar_recursos_nivel(self);
}


private void nivel_move_enemigos(tad_nivel* self){
		//Por cada enemigo del nivel se crea un hilo
		//luego es responsabilidad de cada hilo mover a los enemigos
		foreach(enemigo, self->enemigos, tad_enemigo*){
			thread_free_begin(nuevo_hilo_enemigo, 2,self,enemigo);
		}
}

private void nuevo_hilo_enemigo(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self,tad_enemigo* enemigo);

	logger_info(get_logger(self), "Los enemigos, se mueven en forma de L");

	//teniendo en cuenta que no:
	     //salga del mapa
	     //pase por arriba de una caja
	movimiento_permitido_enemigo(self, enemigo);

}
/*
private void algotirmo_vereficador_deadlock_activate(tad_nivel* self){
	logger_info(get_logger(self), "Se inicia el vereficador deadlock ");
	verificador_deadlock(self);

}

*/
private void nivel_ejecutar_logica(tad_nivel* self){
	var(config_path, get_config_path(self));
	var(socket, self->socket);

	//Creamos un notificador de cambios sobre el archivo de configuracion
	tad_notifier* notifier = notifier_create(config_path);

	//Creamos un multiplexor y le asociamos el notificador y el socket del planificador
	tad_multiplexor* multiplexor = multiplexor_create();
	multiplexor_bind_notifier(multiplexor, notifier, config_file_modified, self);
	multiplexor_bind_socket(multiplexor, socket, manejar_paquete_planificador, self);

	//Redeclaro la funcion manejadora de sigint, para que cierre el multiplexor
	signal_dynamic_handler(SIGINT, nivel_finalizar_cerrar_multiplexor(self, multiplexor));

	//Si el socket pierde la conexion...
	SOCKET_ON_ERROR(socket, manejar_desconexion_multiplexor(self, multiplexor));

	//Esperamos por paquetes entrantes (o cambios en el config file)
	while(1)
		multiplexor_wait_for_io(multiplexor);
}

private void nivel_finalizar(tad_nivel* self){
	logger_info(get_logger(self), "Fin del proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar();
	destruir_nivel(self);

	logger_dispose();

	exit(EXIT_SUCCESS);
}

/***************************************************************
 * Manejo de paquetes entrantes
 ***************************************************************/

private void manejar_paquete_planificador(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self);

	var(socket, self->socket);

	tad_package* paquete = socket_receive_one_of_this_packages(socket, 5,
			PERSONAJE_CONECTADO,
			SOLICITUD_UBICACION_RECURSO,
			PERSONAJE_MOVIMIENTO,
			PERSONAJE_SOLICITUD_RECURSO,
			PERSONAJE_FINALIZO_NIVEL);
	var(tipo, package_get_data_type(paquete));


	if(tipo == PERSONAJE_CONECTADO){
		char simbolo;
		vector2 pos;
		package_get_char_and_vector2(paquete, out simbolo, out pos);

		nivel_gui_crear_personaje(simbolo, pos);

		alloc(personaje, tad_personaje);
		personaje->simbolo = simbolo;
		personaje->pos = pos;
		list_add(self->personajes, personaje);

		nivel_gui_dibujar();

	}else if(tipo == SOLICITUD_UBICACION_RECURSO){
		char recurso = package_get_char(paquete);
		vector2 ubicacion;
		foreach(caja, self->cajas, tad_caja*)
			if(caja->simbolo == recurso)
				ubicacion = caja->pos;
		socket_send_vector2(socket, UBICACION_RECURSO, ubicacion);

	}else if(tipo == PERSONAJE_MOVIMIENTO){
		char simbolo;
		vector2 pos;
		package_get_char_and_vector2(paquete, out simbolo, out pos);
		foreach(personaje, self->personajes, tad_personaje*)
			if(personaje->simbolo == simbolo)
				personaje->pos = pos;
		nivel_gui_mover_item(simbolo, pos);
		nivel_gui_dibujar();

	}else if(tipo == PERSONAJE_SOLICITUD_RECURSO){
		char simbolo;
		char recurso;
		package_get_two_chars(paquete, out simbolo, out recurso);
		//TODO validar que el personaje este en el lugar
		//TODO verificar que halla instancias de ese recurso
		//TODO descontar una instancia a esa caja
		//TODO
		socket_send_char(socket, RECURSO_OTORGADO, simbolo); //hardcod

	}else if(tipo == PERSONAJE_FINALIZO_NIVEL){
		char simbolo = package_get_char(paquete);
		nivel_gui_quitar_personaje(simbolo);
		nivel_gui_dibujar();

	}


}

private void config_file_modified(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self, tad_socket* socket, char* config_file);

	char* algoritmo_actual = self->algoritmo;
	int quantum_actual = self->quantum;
	int retardo_actual = self->retardo;

	char* nuevo_algoritmo;
	int nuevo_quantum;
	int nuevo_retardo;

	t_config* config = config_create(config_file); //TODO llevar esto a nivel_configuracion.c

	cargar_configuracion_cambiante(self, config,
			out nuevo_algoritmo, out nuevo_quantum, out nuevo_retardo);

	config_destroy(config);

	if(quantum_actual != nuevo_quantum){
		self->quantum = nuevo_quantum;
		socket_send_int(socket, QUANTUM, nuevo_quantum);
	}
	if(retardo_actual != nuevo_retardo){
		self->retardo = nuevo_retardo;
		socket_send_int(socket, RETARDO, nuevo_retardo);
	}
	if(!string_equals(algoritmo_actual, nuevo_algoritmo)){
		self->algoritmo = nuevo_algoritmo;
		free(algoritmo_actual);
		socket_send_string(socket, ALGORITMO, nuevo_algoritmo);
	}else{
		free(nuevo_algoritmo);
	}
}


/***************************************************************
 * Manejo de desconexiones
 ***************************************************************/

private void manejar_desconexion(tad_nivel* self){
	logger_error(get_logger(self), "Se cierra la conexion con Plataforma de manera inesperada");
	nivel_finalizar(self);
}

private void manejar_desconexion_multiplexor(tad_nivel* self, tad_multiplexor* m){
	logger_error(get_logger(self), "Se cierra la conexion con Plataforma de manera inesperada");
	nivel_finalizar_cerrar_multiplexor(self, m);
}

private void nivel_finalizar_cerrar_multiplexor(tad_nivel* self, tad_multiplexor* m){
	multiplexor_dispose_and_dispose_objects(m);
	self->socket = null;
	nivel_finalizar(self);
}
