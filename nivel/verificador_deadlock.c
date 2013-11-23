#include <unistd.h>
#include "../libs/common/collections/list.h"
#include "../libs/common/string.h"
#include "../libs/thread/mutex.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/protocol/protocol.h"
#include "verificador_deadlock.h"


void liberar_recursos_del_personaje(tad_personaje* personaje, t_list* recursos_disponibles) {

	foreach (recurso_personaje, personaje->recursos_asignados, tad_recurso*){

		tad_recurso* recurso_p = recurso_personaje;

		int encontre_recurso (tad_recurso* recurso_aux){
			if (recurso_p->simbolo == recurso_aux->simbolo)
				return 1;
			else
				return 0;
		}

		tad_recurso* recurso_aux = list_find (recursos_disponibles, (void*) encontre_recurso);
		recurso_aux->cantidad = recurso_aux->cantidad + recurso_personaje->cantidad;
	}
	list_clean(personaje->recursos_asignados);
}


void verificador_deadlock(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* nivel);

	var(semaforo_personajes, nivel->semaforo_personajes);
	var(semaforo_cajas, nivel->semaforo_cajas);

	bool hay_deadlock;
	int flag_cambios = 0;

	t_list* personajes_deadlock = list_create();
	t_list* lista_personajes = list_create();
	t_list* recursos_disponibles = list_create();


		logger_info(get_logger(nivel),
			"Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
			nivel->tiempo_deadlock / 10000.0);


	while (true) {

		sleep(nivel->tiempo_deadlock);

		hay_deadlock = false;
		bool hay_bloqueados = false;

		//semaforo para acceder a lista de personajes del nivel
		mutex_close(semaforo_personajes);

		//agrega los personajes del nivel a lista auxiliar lista_personajes
		foreach (personaje_nivel, nivel->personajes, tad_personaje*){
			alloc (personaje, tad_personaje);
			personaje->nombre = personaje_nivel->nombre;
			personaje->pos = personaje_nivel->pos;
			personaje->recurso_pedido = personaje_nivel->recurso_pedido;
			personaje->recursos_asignados = personaje_nivel->recursos_asignados;
			personaje->simbolo = personaje_nivel->simbolo;
			list_add (lista_personajes, personaje);
		}

		mutex_open(semaforo_personajes);


		//agrega los recursos del nivel a lista auxiliar recursos_disponibles

		if (list_is_empty(lista_personajes)==false){

			//semaforo para acceder a lista de cajas del nivel
			mutex_close(semaforo_cajas);

			foreach (caja, nivel->cajas, tad_caja*){
				alloc (recurso, tad_recurso);
				recurso->simbolo = caja->simbolo;
				recurso->cantidad = caja->instancias;
				list_add (recursos_disponibles, recurso);
			}

			mutex_open(semaforo_cajas);

		}

		//libera recursos de los personajes que no estan bloqueados
		foreach(personaje, lista_personajes, tad_personaje*){
			if (personaje->recurso_pedido->cantidad == 0){
				if (list_size(personaje->recursos_asignados) > 0){
				liberar_recursos_del_personaje(personaje, recursos_disponibles);
				}
			}
			else{
				hay_bloqueados = true;
			}
		}

		//si hay personajes bloqueados

		if (hay_bloqueados){

			do {
				flag_cambios = 0;

				foreach(personaje, lista_personajes, tad_personaje*){
					//para los personajes bloqueados con recursos asignados verifico si puedo otorgar el recurso solicitado a aquellos
					if (list_size(personaje->recursos_asignados) > 0) {

						//busca el recurso solicitado en la lista de recursos disponibles
						tad_recurso* recurso_p;
						recurso_p->simbolo = personaje->recurso_pedido->simbolo;
						recurso_p->cantidad = 1;

						int encontre_recurso (tad_recurso* caja_recurso){
							if (recurso_p->simbolo == caja_recurso->simbolo)
								return 1;
							else
								return 0;
						}

						tad_recurso* caja_recurso= list_find(recursos_disponibles, (void*) encontre_recurso);

						//si puedo otorgar el recurso solicitado libero los recursos asignados
						if(caja_recurso->cantidad > 0) {
							liberar_recursos_del_personaje(personaje, recursos_disponibles);
							personaje->recurso_pedido->cantidad = 0;
							personaje->recurso_pedido->simbolo = '\0';
							flag_cambios = 1;
						}
					}
				}
				//si libere recursos vuelvo a recorrer la lista para ver si puedo desbloquear algun personaje
			} while (flag_cambios != 0);


			char *str_personajes_deadlock = string_new();

			//Identifico los personajes en deadlock por tener recurso pedido (bloqueado) y recursos asignados y los cargo en una nueva lista
			foreach (personaje, lista_personajes, tad_personaje*){
				if (personaje->recurso_pedido->cantidad == 1
						&& list_size(personaje->recursos_asignados) > 0) {
					alloc (personaje_bloqueado, t_personaje_bloqueado);
					personaje_bloqueado->simbolo = personaje->simbolo;
					personaje_bloqueado->nombre = personaje->nombre;
					list_add (personajes_deadlock, personaje_bloqueado);
					string_append_with_format(&str_personajes_deadlock, "%s.", personaje_bloqueado->nombre);
				}
			}

			//si existen por lo menos dos personajes en la lista entonces hay deadlock
			if (list_size(personajes_deadlock) > 1)
				hay_deadlock = true;

			if (hay_deadlock){

				//informar deadlock por archivo log indicando personajes involucrados;
				logger_info(get_logger(nivel),
						"Se detecto deadlock. Personajes involucrados: %s", str_personajes_deadlock);

				//si el recovery esta activado elijo personaje victima e informo al planificador
				if (nivel->recovery == 1) {

					//elijo como victima al primer personaje de la lista
					tad_personaje* personaje_victima = list_get(personajes_deadlock, 1);

					//informo por archivo de log
					logger_info(get_logger(nivel), "El personaje %s ha sido seleccionado como victima del deadlock", personaje_victima->nombre);

					//informo muerte del personaje por deadlock al planificador
					socket_send_char(nivel->socket, MUERTE_POR_DEADLOCK, personaje_victima->simbolo);

				}

			}
			dealloc(str_personajes_deadlock);
		}

		//libero personajes en deadlock
		list_clean(personajes_deadlock);
		//libero recursos disponibles
		list_clean(recursos_disponibles);
		//libero personajes del nivel
		list_clean(lista_personajes);

	}

	list_destroy(personajes_deadlock);
	list_destroy(recursos_disponibles);
	list_destroy(lista_personajes);

}
