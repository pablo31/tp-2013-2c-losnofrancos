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

private void nivel_crear_hilos_enemigos(tad_nivel* self);
private void nivel_crear_hilo_deadlock(tad_nivel* self);
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
	nivel_crear_hilos_enemigos(self);

	//algoritmo verificador de deadlock
	nivel_crear_hilo_deadlock(self);

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

	self->semaforo_bloqueados = mutex_create();
	self->bloqueados = list_create();

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
	free(ip);
	free(puerto);
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


void nivel_crear_hilos_enemigos(tad_nivel* self){
		//Por cada enemigo del nivel se crea un hilo
		//luego es responsabilidad de cada hilo mover a los enemigos

		int rows, cols;
		nivel_gui_get_area_nivel(out rows, out cols);
		logger_info(get_logger(self), "Mapa del juego: %d filas y %d columnas", rows, cols);

	    int i=0;
		foreach(enemigo, self->enemigos, tad_enemigo*){
			i++;
			logger_info(get_logger(self), "Posicion enemigo %d: en (%d:%d)", i, enemigo->pos.x,enemigo->pos.y);
			thread_free_begin(enemigo_ia, 2, self, enemigo);

		}
}

private void nivel_crear_hilo_deadlock(tad_nivel* self){
	logger_info(get_logger(self), "Se inicia el verificador de deadlocks");
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

	//Esperamos por paquetes entrantes o cambios en el config file
	while(1)
		multiplexor_wait_for_io(multiplexor);
}


private void nivel_finalizar(tad_nivel* self){
	logger_info(get_logger(self), "Fin del proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar();
	destruir_nivel(self);

	logger_dispose();

	signal_dispose_all();

	exit(EXIT_SUCCESS);
}

/***************************************************************
 * Manejo de paquetes entrantes
 ***************************************************************/

private void manejar_paquete_planificador(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self);

	var(socket, self->socket);
	var(logger, self->logger);


	mutex_close(self->semaforo_personajes);


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
		alloc(recurso, tad_recurso);
		personaje->simbolo = simbolo;
		personaje->nombre = nombre;
		personaje->pos = pos;
		recurso->cantidad = 0;
		recurso->simbolo = '\0';
		personaje->recurso_pedido = recurso;
		personaje->recursos_asignados = list_create();

		list_add(self->personajes, personaje);

		logger_info(logger, "Se cargo configuracion nivel");


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

		//Se busca personaje en lista personajes
		bool personaje_buscado(tad_personaje* ptr){
			return ptr->simbolo == simbolo;
		}
		tad_personaje* personaje_en_movimiento = list_find(self->personajes, (void*)personaje_buscado);

		if(personaje_en_movimiento != null){
			personaje_en_movimiento->pos = pos;
			logger_info(get_logger(self), "Personaje %s se mueve a (%d,%d)", personaje_en_movimiento->nombre, pos.x, pos.y);

			mutex_close(self->semaforo_enemigos);
			foreach(enemigo, self->enemigos, tad_enemigo*){
				if(vector2_equals(enemigo->pos, personaje_en_movimiento->pos)){
					logger_info(enemigo->logger, "El personaje %s fue atrapado por un enemigo", personaje_en_movimiento->nombre);
					muerte_del_personaje(personaje_en_movimiento->simbolo, self, ENEMIGO);
				}
			}
			mutex_open(self->semaforo_enemigos);
		}



	}else if(tipo == PERSONAJE_SOLICITUD_RECURSO){
		char simbolo_personaje;
		char simbolo_recurso;
		package_get_two_chars(paquete, out simbolo_personaje, out simbolo_recurso);

		//Se busca personaje
		bool personaje_buscado(tad_personaje* ptr){
			return ptr->simbolo == simbolo_personaje;
		}
		tad_personaje* personaje_solicitud = list_find(self->personajes, (void*)personaje_buscado);

		if(personaje_solicitud != null) evaluar_solicitud_recurso(self, personaje_solicitud, simbolo_recurso);


	}else if(tipo == PERSONAJE_DESCONEXION){
		char simbolo = package_get_char(paquete);

		//Se busca personaje en lista personajes
		bool personaje_buscado(tad_personaje* ptr){
			return ptr->simbolo == simbolo;
		}
		tad_personaje* personaje_fin = list_find(self->personajes, (void*)personaje_buscado);

		if(personaje_fin != null){
			logger_info(get_logger(self), "Se informa desconexion del personaje %s", personaje_fin->nombre);
			muerte_del_personaje(simbolo, self, FIN);
		}

	}


	mutex_open(self->semaforo_personajes);
	nivel_gui_dibujar(self);
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


void evaluar_solicitud_recurso(tad_nivel* self, tad_personaje* personaje_solicitud, char simbolo_recurso){

	logger_info(get_logger(self), "El personaje %s solicita recurso %c. Cantidad de recursos asignados al momento: %d ", personaje_solicitud->nombre, simbolo_recurso, list_size(personaje_solicitud->recursos_asignados));
	//logger_info(get_logger(self), "Cantidad de recursos asignados al momento: %d", list_size(personaje_solicitud->recursos_asignados));
	alloc(recurso_pedid, tad_recurso);
	recurso_pedid->simbolo = simbolo_recurso;
	recurso_pedid->cantidad = 1;
	personaje_solicitud->recurso_pedido = recurso_pedid;

	alloc(personaje_bloqueado, tad_bloqueado);
	personaje_bloqueado->simbolo = personaje_solicitud->simbolo;
	personaje_bloqueado->recurso = personaje_solicitud->recurso_pedido->simbolo;
	mutex_close(self->semaforo_bloqueados);
	list_add(self->bloqueados, personaje_bloqueado);
	mutex_open(self->semaforo_bloqueados);

	bool caja_buscada(tad_caja* ptr){
		return ptr->simbolo == simbolo_recurso;
	}

	mutex_close(self->semaforo_cajas);
	tad_caja* recurso_caja = list_find(self->cajas, (void*)caja_buscada);
	var(instancias_caja, recurso_caja->instancias);

	//Se verifica que haya instancias del recurso para otorgar
	if (instancias_caja > 0){
		recurso_caja->instancias --;

		//logger_info(get_logger(self), "Se puede otorgar el recurso %c", recurso_caja->simbolo);
		otorgar_recurso(self, personaje_solicitud->simbolo, simbolo_recurso);
	}
	else logger_info(get_logger(self), "No se puede otorgar el recurso %c al personaje %s", recurso_caja->simbolo, personaje_solicitud->nombre);
	mutex_open(self->semaforo_cajas);
}

void otorgar_recurso(tad_nivel* self, char simbolo_personaje, char simbolo_recurso){
	bool encontre_recurso = false;
	//actualizar lista recursos_asignados del personaje aumentando cant.del recurso
	//o agregando un nuevo recurso a la lista

	//Se busca personaje
	bool personaje_buscado(tad_personaje* ptr){
		return ptr->simbolo == simbolo_personaje;
	}
	tad_personaje* personaje_solicitud = list_find(self->personajes, (void*)personaje_buscado);

	//buscar en lista recursos_asignados del personaje
	var(recursos_asignados, personaje_solicitud->recursos_asignados);
	foreach(recurso_personaje, recursos_asignados, tad_recurso*){
		//si lo encuentra incrementa la cantidad
		if(recurso_personaje->simbolo == simbolo_recurso){
			recurso_personaje->cantidad ++;
			encontre_recurso = true;
		}
	}

	//if(encontre_recurso == true)
		//logger_info(get_logger(self), "El personaje %c ya tenia el recurso %c, se incrementa cantidad", simbolo_personaje, simbolo_recurso);

	//si no lo encontro lo agrega a la lista de recursos asignados
	if(!encontre_recurso){
		//logger_info(get_logger(self), "El personaje %c no tenia el recurso %c, se agrega", simbolo_personaje, simbolo_recurso);
		alloc(nuevo_recurso, tad_recurso);
		nuevo_recurso->simbolo = simbolo_recurso;
		nuevo_recurso->cantidad = 1;
		list_add(personaje_solicitud->recursos_asignados, nuevo_recurso);
	}

	//actualizar recurso_pedido del personaje
	personaje_solicitud->recurso_pedido->simbolo = '\0';
	personaje_solicitud->recurso_pedido->cantidad = 0;
	var(personaje_simbolo, personaje_solicitud->simbolo);

    bool bloqueado_buscado(tad_bloqueado* ptr){
            return ptr->simbolo == personaje_simbolo;
    }
    mutex_close(self->semaforo_bloqueados);
	tad_bloqueado* personaje_desbloqueado = list_remove_by_condition(self->bloqueados, (void*)bloqueado_buscado);
	mutex_open(self->semaforo_bloqueados);
	free(personaje_desbloqueado);

	logger_info(get_logger(self), "Se otorga el recurso %c al personaje %s", simbolo_recurso, personaje_solicitud->nombre);
	socket_send_char(self->socket, RECURSO_OTORGADO, personaje_simbolo);
}


void liberar_y_reasignar_recursos(tad_nivel* self, tad_personaje* personaje_muerto) {

	logger_info(get_logger(self), "Cantidad de recursos a liberar por personaje %s: %d", personaje_muerto->nombre, list_size(personaje_muerto->recursos_asignados));

	//por cada recurso que tenia asignado el personaje
	foreach(recurso_a_liberar, personaje_muerto->recursos_asignados, tad_recurso*){

		//voy recorriendo la lista de bloqueados y si aguardaba ese recurso se le otorga mientras queden instancias disponibles

		foreach(personaje_bloqueado, self->bloqueados, tad_bloqueado*){

			if(recurso_a_liberar->cantidad > 0){
				mutex_close(self->semaforo_bloqueados);
				var(recurso_solicitado, personaje_bloqueado->recurso);
				var(simbolo_bloqueado, personaje_bloqueado->simbolo);
				mutex_open(self->semaforo_bloqueados);
				var(recurso_liberado, recurso_a_liberar->simbolo);

				if(recurso_solicitado == recurso_liberado){
					//logger_info(get_logger(self), "recurso solicitado por un personaje bloqueado");
					otorgar_recurso(self, simbolo_bloqueado, recurso_solicitado);
					recurso_a_liberar->cantidad --;
					//logger_info(get_logger(self), "Se desbloquea el personaje %c. Se le otorga el recurso %c", personaje_muerto->simbolo, recurso_solicitado);
				}
			}
		}


		//si luego sobraron recursos se actualiza las instancias en la lista de cajas del nivel
		if(recurso_a_liberar->cantidad > 0){
			tad_recurso* recurso_p = recurso_a_liberar;

			bool caja_buscada (tad_caja* caja){
				return (recurso_p->simbolo == caja->simbolo);
			}
			mutex_close(self->semaforo_cajas);
			tad_caja* caja = list_find (self->cajas, (void*)caja_buscada);
			caja->instancias += recurso_a_liberar->cantidad;
			mutex_open(self->semaforo_cajas);

			logger_info(get_logger(self), "Se libero el recurso %c, instancias %d. Se actualiza la lista de cajas del nivel", caja->simbolo, recurso_a_liberar->cantidad);
		}
	}

	//list_destroy(lista_bloqueados);
	logger_info(get_logger(self), "Se liberaron todos los recursos del personaje %s", personaje_muerto->nombre);
}


void muerte_del_personaje(char personaje_simbolo, tad_nivel* self, int motivo){

        //Se elimina personaje de la lista
        bool personaje_buscado(tad_personaje* ptr){
                return ptr->simbolo == personaje_simbolo;
        }
    	bool bloqueado_buscado(tad_bloqueado* ptr){
    		return ptr->simbolo == personaje_simbolo;
    	}

    	void eliminar_personaje(tad_personaje* personaje){
    	    free(personaje->nombre);
    	    free(personaje->recurso_pedido);
    	    list_clean_and_destroy_elements(personaje->recursos_asignados, free);
    	    free(personaje);
    	}

    	tad_personaje* personaje_muerto = list_remove_by_condition(self->personajes, (void*)personaje_buscado);

    	if(personaje_muerto == null) return;

    	if(motivo == ENEMIGO)
    		logger_info(get_logger(self), "Muerte por enemigo del personaje %s, se elimina de la lista del nivel", personaje_muerto->nombre);
    	if(motivo == DEADLOCK)
    	    logger_info(get_logger(self), "Muerte por deadlock del personaje %s, se elimina de la lista del nivel", personaje_muerto->nombre);


    	//lo elimino de la lista de bloqueados

    	mutex_close(self->semaforo_bloqueados);
    	tad_bloqueado* personaje_desbloqueado = list_remove_by_condition(self->bloqueados, (void*)bloqueado_buscado);
    	mutex_open(self->semaforo_bloqueados);
    	if(personaje_desbloqueado != null)
    		free(personaje_desbloqueado);

       	//Se liberan recursos asignados y se reasignan a los personajes bloqueados
       	liberar_y_reasignar_recursos(self, personaje_muerto);

        if(motivo == ENEMIGO)
             socket_send_char(self->socket, MUERTE_POR_ENEMIGO, personaje_simbolo);
        if(motivo == DEADLOCK)
            socket_send_char(self->socket, MUERTE_POR_DEADLOCK, personaje_simbolo);

        eliminar_personaje(personaje_muerto);
}


