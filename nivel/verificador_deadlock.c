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

/* Logica algoritmo de deteccion de deadlock
 *
 * Se carga la lista de personajes actuales del nivel y la lista de cajas del nivel en dos lista auxiliares.
 * Cada personaje conoce su lista de recursos asignados hasta el momento y
 * su solicitud de recurso actual si es que hubiese.
 * Primero se detectan los personajes que no tiene ninguna solicitud de recurso actual,
 * es decir, no estan bloqueados, y que por lo tanto no estaran involucrados en el deadlock,
 * y se liberan sus recursos asignados cargandolos en la lista auxiliar de recursos disponibles.
 * Si hay personajes bloqueados entonces se recorre la lista:
 * aquellos que no tienen recursos asignados no entraran en deadlock,
 * si tienen recursos y hay 1 instancia al menos del recurso se asume que se otorga la solicitud
 * y se liberan los recursos asignados, por lo que se recorre nuevamente la lista de personajes
 * para controlar si se puede desbloquear alguno a partir de los recursos liberados en la anterior vuelta.
 * Si hay almenos dos personajes que tienen recurso pedido (estan bloqueados) y a la vez recursos asignados,
 * entonces significa que hay deadlock.
 * Si el nivel tiene el recovery activado se elige como victima al personaje que haya ingresado primero al nivel
 */

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

		//Se agregan los personajes del nivel a lista auxiliar lista_personajes
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


		//Se agregan los recursos del nivel a lista auxiliar recursos_disponibles

		if (list_is_empty(lista_personajes)==false){

			//Semaforo para acceder a lista de cajas del nivel
			mutex_close(semaforo_cajas);

			foreach (caja, nivel->cajas, tad_caja*){
				alloc (recurso, tad_recurso);
				recurso->simbolo = caja->simbolo;
				recurso->cantidad = caja->instancias;
				list_add (recursos_disponibles, recurso);
			}

			mutex_open(semaforo_cajas);

		}


		//Se liberan recursos de los personajes que no estan bloqueados
		foreach(personaje, lista_personajes, tad_personaje*){
			var(solicitud_recurso, personaje->recurso_pedido->cantidad);
			if (solicitud_recurso == 0){
				if (list_size(personaje->recursos_asignados) > 0){
				liberar_recursos_del_personaje(personaje, recursos_disponibles);
				}
			}
			else{
				hay_bloqueados = true;
			}
		}

		//Si hay personajes bloqueados

		if (hay_bloqueados){

			do {
				flag_cambios = 0;

				foreach(personaje, lista_personajes, tad_personaje*){

					var(recursos_asignados, personaje->recursos_asignados);

					//Para los personajes bloqueados con recursos asignados se verifica si se puede otorgar el recurso solicitado
					if (list_size(recursos_asignados) > 0) {

						//Se busca el recurso solicitado en la lista de recursos disponibles
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

						//Si se puede otorgar el recurso solicitado se liberan los recursos asignados
						if(caja_recurso->cantidad > 0) {
							liberar_recursos_del_personaje(personaje, recursos_disponibles);
							personaje->recurso_pedido->cantidad = 0;
							personaje->recurso_pedido->simbolo = '\0';
							flag_cambios = 1;
						}
					}
				}
				//Si se liberaron recursos se vuelve a recorrer la lista para ver si se puede desbloquear algun personaje
			} while (flag_cambios != 0);


			char *str_personajes_deadlock = string_new();

			//Se dentifican los personajes en deadlock por tener recurso pedido (bloqueado) y recursos asignados y se cargan en una nueva lista
			foreach (personaje, lista_personajes, tad_personaje*){
				var(solicitud_recurso, personaje->recurso_pedido->cantidad);
				var(recursos_asignados, personaje->recursos_asignados);
				if (solicitud_recurso == 1 && list_size(recursos_asignados) > 0) {
					alloc (personaje_bloqueado, t_personaje_bloqueado);
					personaje_bloqueado->simbolo = personaje->simbolo;
					personaje_bloqueado->nombre = personaje->nombre;
					list_add (personajes_deadlock, personaje_bloqueado);
					string_append_with_format(&str_personajes_deadlock, "%s.", personaje_bloqueado->nombre);
				}
			}

			//Si existen por lo menos dos personajes en la lista entonces hay deadlock
			if (list_size(personajes_deadlock) > 1)
				hay_deadlock = true;

			if (hay_deadlock){

				//Se informa deadlock por archivo log indicando personajes involucrados;
				logger_info(get_logger(nivel),
						"Se detecto deadlock. Personajes involucrados: %s", str_personajes_deadlock);

				//Si el recovery esta activado se elige personaje victima y se informa al planificador
				if (nivel->recovery == 1) {

					//Se elige como victima al primer personaje de la lista
					tad_personaje* personaje_victima = list_get(personajes_deadlock, 1);

					//Seinforma por archivo de log
					logger_info(get_logger(nivel), "El personaje %s ha sido seleccionado como victima del deadlock", personaje_victima->nombre);

					//Se informa muerte del personaje por deadlock al planificador
					socket_send_char(nivel->socket, MUERTE_POR_DEADLOCK, personaje_victima->simbolo);

				}

			}
			dealloc(str_personajes_deadlock);
		}

		list_clean(personajes_deadlock);
		list_clean(recursos_disponibles);
		list_clean(lista_personajes);

	}

	list_destroy(personajes_deadlock);
	list_destroy(recursos_disponibles);
	list_destroy(lista_personajes);

}
