#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../libs/socket/socket_utils.h"
#include "../libs/socket/package_serializers.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/notifier/notifier.h"
#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/thread/mutex.h"
#include "../libs/protocol/protocol.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"
#include "enemigo.h"
#include "verificador_deadlock.h"

tad_nivel* crear_nivel(char* config_path, char* as_out ippuerto);

private void nivel_conectar_a_plataforma(tad_nivel* self, char* ippuerto);
private void nivel_iniciar_interfaz_grafica(tad_nivel* self);

private void nivel_move_enemigos(tad_nivel* self);
private void nivel_crea_hilo_deadlock(tad_nivel* self);
private void nivel_ejecutar_logica(tad_nivel* self);

private void manejar_desconexion(tad_nivel* self);
private void manejar_desconexion_multiplexor(tad_nivel* self, tad_multiplexor* m);
private void manejar_paquete_planificador(PACKED_ARGS);
private void modificacion_archivo_config(PACKED_ARGS);

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

	//algoritmo verificador de deadlock
	nivel_crea_hilo_deadlock(self);

	//ejecutamos la logica
	nivel_ejecutar_logica(self);


	//no deberia llegar hasta aca, pero por si las dudas...
	nivel_finalizar(self);
	return EXIT_FAILURE;
}


tad_nivel* crear_nivel(char* config_path, char* as_out ippuerto){
	alloc(self, tad_nivel);

	self->config_path = config_path;

	self->semaforo_personajes = mutex_create();
	self->personajes = list_create();

	self->semaforo_cajas = mutex_create();
	self->cajas = list_create();

	self->semaforo_enemigos = mutex_create();
	self->enemigos = list_create();

	char* plataforma;
	cargar_configuracion_nivel(self, out plataforma);
	set ippuerto = plataforma;

	return self;
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
	nivel_gui_dibujar(self);
}


void nivel_move_enemigos(tad_nivel* self){
		//Por cada enemigo del nivel se crea un hilo
		//luego es responsabilidad de cada hilo mover a los enemigos

	    int i=0;
		foreach(enemigo, self->enemigos, tad_enemigo*){
			i++;
			logger_info(get_logger(self), "posicon enemigo %d: en (%d:%d)", i,enemigo->pos.x,enemigo->pos.y);
			thread_free_begin(movimiento_permitido_enemigo, 2,self,enemigo);

		}
}




private void nivel_crea_hilo_deadlock(tad_nivel* self){
	logger_info(get_logger(self), "Se inicia el vereficador deadlock ");
	thread_free_begin(verificador_deadlock, 1, self);
}


private void nivel_ejecutar_logica(tad_nivel* self){
	var(config_path, get_config_path(self));
	var(socket, self->socket);

	//Creamos un notificador de cambios sobre el archivo de configuracion
	tad_notifier* notifier = notifier_create(config_path);

	//Creamos un multiplexor y le asociamos el notificador y el socket del planificador
	tad_multiplexor* multiplexor = multiplexor_create();
	multiplexor_bind_notifier(multiplexor, notifier, modificacion_archivo_config, self, notifier);
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
	var(logger, self->logger);

	tad_package* paquete = socket_receive_one_of_this_packages(socket, 5,
			PERSONAJE_CONECTADO,
			SOLICITUD_UBICACION_RECURSO,
			PERSONAJE_MOVIMIENTO,
			PERSONAJE_SOLICITUD_RECURSO,
			PERSONAJE_DESCONEXION);
	var(tipo, package_get_data_type(paquete));


	if(tipo == PERSONAJE_CONECTADO){
		char simbolo = socket_receive_expected_char(socket, PERSONAJE_SIMBOLO);
		char* nombre = socket_receive_expected_string(socket, PERSONAJE_NOMBRE);
		vector2 pos = socket_receive_expected_vector2(socket, PERSONAJE_POSICION);

		logger_info(logger, "El personaje %s entro al nivel", nombre);

		alloc(personaje, tad_personaje);
		personaje->simbolo = simbolo;
		personaje->nombre = nombre;
		personaje->pos = pos;
		personaje->recurso_pedido = null;
		personaje->recursos_asignados = list_create();

		mutex_close(self->semaforo_personajes);
		list_add(self->personajes, personaje);
		mutex_open(self->semaforo_personajes);

		nivel_gui_dibujar(self);

	}else if(tipo == SOLICITUD_UBICACION_RECURSO){
		char recurso = package_get_char(paquete);
		vector2 ubicacion;

		mutex_close(self->semaforo_cajas);

		foreach(caja, self->cajas, tad_caja*)
			if(caja->simbolo == recurso)
				ubicacion = caja->pos;

		mutex_open(self->semaforo_cajas);

		socket_send_vector2(socket, UBICACION_RECURSO, ubicacion);

	}else if(tipo == PERSONAJE_MOVIMIENTO){
		char simbolo;
		vector2 pos;
		package_get_char_and_vector2(paquete, out simbolo, out pos);

		tad_personaje* personaje_en_movimiento;

		mutex_close(self->semaforo_personajes);
		foreach(personaje, self->personajes, tad_personaje*){
			if(personaje->simbolo == simbolo){
				personaje->pos = pos;
				personaje_en_movimiento = personaje;
			}
		}

		//controlar si al moverse fue atrapado por un enemigo
		mutex_close(self->semaforo_enemigos);
		foreach (enemigo, self->enemigos, tad_enemigo*){
			verificar_muerte_por_enemigo(personaje_en_movimiento, enemigo->pos, self);
		}
		mutex_open(self->semaforo_enemigos);
		mutex_open(self->semaforo_personajes);

		nivel_gui_dibujar(self);

	}else if(tipo == PERSONAJE_SOLICITUD_RECURSO){
		char simbolo_personaje;
		char simbolo_recurso;

		package_get_two_chars(paquete, out simbolo_personaje, out simbolo_recurso);

		evaluar_solicitud_recurso(self,simbolo_personaje,simbolo_recurso);

	}else if(tipo == PERSONAJE_DESCONEXION){
		char simbolo = package_get_char(paquete);

		logger_info(get_logger(self), "Se desconecto el personaje %c del %s", simbolo, self->nombre);

		bool personaje_buscado(void* ptr){
			return ((tad_personaje*)ptr)->simbolo == simbolo;
		}

		//Se busca personaje en lista personajes
		mutex_close(self->semaforo_personajes);
		tad_personaje* personaje_fin = list_find(self->personajes, personaje_buscado);

		//Se liberan recursos asignados y se reasignan a los personajes bloqueados
		mutex_close(self->semaforo_cajas);
		liberar_y_reasignar_recursos(self, personaje_fin);
		mutex_open(self->semaforo_cajas);

		//Se elimina personaje de la lista
		list_remove_by_condition(self->personajes, personaje_buscado);
		mutex_open(self->semaforo_personajes);

		nivel_gui_dibujar(self);

	}


}

private void modificacion_archivo_config(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self, tad_notifier* notifier);

	var(socket, self->socket);
	var(config_file, self->config_path);
	var(logger, get_logger(self));

	char* algoritmo_actual = self->algoritmo;
	int quantum_actual = self->quantum;
	int retardo_actual = self->retardo;

	char* nuevo_algoritmo;
	int nuevo_quantum;
	int nuevo_retardo;

	logger_info(logger, "Archivo de configuracion modificado");
	notifier_wait_for_modification(notifier);

	recargar_configuracion_nivel(self, config_file,
			out nuevo_algoritmo, out nuevo_quantum, out nuevo_retardo);

	int cambios = 0;

	if(quantum_actual != nuevo_quantum){
		self->quantum = nuevo_quantum;
		socket_send_int(socket, QUANTUM, nuevo_quantum);
		cambios++;
	}
	if(retardo_actual != nuevo_retardo){
		self->retardo = nuevo_retardo;
		socket_send_int(socket, RETARDO, nuevo_retardo);
		cambios++;
	}
	if(!string_equals(algoritmo_actual, nuevo_algoritmo)){
		self->algoritmo = nuevo_algoritmo;
		free(algoritmo_actual);
		socket_send_string(socket, ALGORITMO, nuevo_algoritmo);
		cambios++;
	}else{
		free(nuevo_algoritmo);
	}

	if(!cambios)
		logger_info(logger, "El archivo de configuracion no presenta cambios");
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


void evaluar_solicitud_recurso(tad_nivel* self, char simbolo_personaje, char simbolo_recurso){

	logger_info(get_logger(self), "Se evalua la solicitud del recurso %c para el personaje %c", simbolo_recurso, simbolo_personaje);

	//Se busca personaje
	bool personaje_buscado(tad_personaje* ptr){
		return ptr->simbolo == simbolo_personaje;
	}
	mutex_close(self->semaforo_personajes);
	tad_personaje* personaje_solicitud = list_find(self->personajes, (void*)personaje_buscado);

	logger_info(get_logger(self), "El personaje %c solicita recurso %c ", simbolo_personaje, simbolo_recurso);

	alloc(recurso_pedid, tad_recurso);
	recurso_pedid->simbolo = simbolo_recurso;
	recurso_pedid->cantidad = 1;
	personaje_solicitud->recurso_pedido = recurso_pedid;
	mutex_open(self->semaforo_personajes);

	mutex_close(self->semaforo_cajas);

	bool caja_buscada(tad_caja* ptr){
		return ptr->simbolo == simbolo_recurso;
	}
	tad_caja* recurso_caja = list_find(self->cajas, (void*)caja_buscada);

	logger_info(get_logger(self), "La caja buscada es: %c", recurso_caja->simbolo);

	//Se verifica que haya instancias del recurso para otorgar
	if (recurso_caja->instancias > 0){

		recurso_caja->instancias --;

		logger_info(get_logger(self), "Se puede otorgar el recurso %c", recurso_caja->simbolo);

		otorgar_recurso(self, personaje_solicitud, simbolo_recurso);

	}

	mutex_open(self->semaforo_cajas);
}

void otorgar_recurso(tad_nivel* self, tad_personaje* personaje_solicitud, char simbolo_recurso){

	//actualizar lista recursos_asignados del personaje aumentando cant.del recurso
	//o agregando un nuevo recurso a la lista

	//buscar en lista recursos_asignados del personaje
	mutex_close(self->semaforo_personajes);
	var(recursos_asignados, personaje_solicitud->recursos_asignados);
	bool encontre_recurso = false;

	foreach (recurso_personaje, recursos_asignados, tad_recurso*){
		//si lo encuentra incrementa la cantidad
		if (recurso_personaje->simbolo == simbolo_recurso){
			logger_info(get_logger(self), "Se encontro recurso %c en lista recursos asignados", recurso_personaje->simbolo);
			recurso_personaje->cantidad ++;
			encontre_recurso = true;
		}
	}

	alloc(nuevo_recurso, tad_recurso);
	//si no lo encontro lo agrega a la lista de recursos asignados
	if (encontre_recurso == false) {
		nuevo_recurso->simbolo = simbolo_recurso;
		nuevo_recurso->cantidad = 1;
		list_add(personaje_solicitud->recursos_asignados, nuevo_recurso);
	}

	//actualizar recurso_pedido del personaje
	personaje_solicitud->recurso_pedido = null;
	mutex_open(self->semaforo_personajes);
	var (personaje_simbolo, personaje_solicitud->simbolo);
	logger_info(get_logger(self), "Se otorga el recurso %c al personaje %c", simbolo_recurso, personaje_simbolo);
	socket_send_char(self->socket, RECURSO_OTORGADO, personaje_simbolo);

}


void liberar_y_reasignar_recursos(tad_nivel* self, tad_personaje* personaje_muerto) {

	logger_info(get_logger(self), "Se liberan los recursos del personaje %c", personaje_muerto->simbolo);

	t_list* lista_personajes = self->personajes;

	bool esta_bloqueado (tad_personaje* personaje){
		return (personaje->recurso_pedido != NULL);
	}

	t_list* lista_bloqueados = list_filter(lista_personajes, (void*) esta_bloqueado);

	//por cada recurso que tenia asignado el personaje
	foreach (recurso_personaje, personaje_muerto->recursos_asignados, tad_recurso*){

		//voy recorriendo la lista de bloqueados y si aguardaba ese recurso se le otorga mientras queden instancias disponibles
		foreach (personaje_bloqueado, lista_bloqueados, tad_personaje*){

			if (recurso_personaje->cantidad > 0) {

				var(recurso_solicitado, personaje_bloqueado->recurso_pedido->simbolo);
				var(recurso_liberado, recurso_personaje->simbolo);

				if (recurso_solicitado == recurso_liberado){
					otorgar_recurso(self, personaje_bloqueado, recurso_solicitado);
					recurso_personaje->cantidad --;
					logger_info(get_logger(self), "Se desbloquea el personaje %c. Se le otorga el recurso %c", personaje_muerto->simbolo, recurso_solicitado);
				}
			}

		}

		//si luego sobraron recursos se actualiza las instancias en la lista de cajas del nivel
		if (recurso_personaje->cantidad > 0){

			tad_recurso* recurso_p = recurso_personaje;

			bool caja_buscada (tad_caja* caja){
				return (recurso_p->simbolo == caja->simbolo);
			}

			tad_caja* caja = list_find (self->cajas, (void*)caja_buscada);

			caja->instancias += recurso_personaje->cantidad;

			logger_info(get_logger(self), "Se actualiza la lista de cajas del nivel");
		}
	}
	list_clean(personaje_muerto->recursos_asignados);
}

void verificar_muerte_por_enemigo(tad_personaje* personaje, vector2 pos_enemigo, tad_nivel* self){

	if (vector2_equals(personaje->pos, pos_enemigo)){

		var(personaje_muerto, personaje->simbolo);

		logger_info(self->logger, "murio el personaje %c al ser atrapado por un enemigo.", personaje_muerto);

		//se avisa la muerte del personaje por enemigo al planificador
		socket_send_char(self->socket, MUERTE_POR_ENEMIGO, personaje_muerto);
	}

}


