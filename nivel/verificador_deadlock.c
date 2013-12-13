#include <unistd.h>
#include "../libs/common/collections/list.h"
#include "../libs/common/string.h"
#include "../libs/thread/mutex.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/protocol/protocol.h"
#include "verificador_deadlock.h"

private void liberar_lista_personajes(t_list* lista);
private void liberar_lista_recursos(t_list* lista);
private void liberar_bloqueado(t_personaje_bloqueado* personaje);
private void liberar_personaje(void* ptr_pj);


void liberar_recursos_del_personaje(tad_personaje* personaje, t_list* recursos_disponibles) {

	foreach(recurso_personaje, personaje->recursos_asignados, tad_recurso*){

		char recurso_aux = recurso_personaje->simbolo;
		bool encontre_recurso (tad_recurso* recurso_disponible){
			return (recurso_aux == recurso_disponible->simbolo);
		}

		tad_recurso* recurso_disponible = list_find(recursos_disponibles, (void*) encontre_recurso);
		recurso_disponible->cantidad += recurso_personaje->cantidad;
	}
	list_clean_and_destroy_elements(personaje->recursos_asignados, free);
}


void resolver_deadlock(tad_nivel* nivel, t_list* personajes_deadlock){

	//Si el recovery esta activado se elige personaje victima y se informa al planificador
	if(nivel->recovery == 1){
		logger_info(get_logger(nivel), "DEADLOCK: El recovery se encuentra activado");

		//Se elige como victima al primer personaje de la lista
		t_personaje_bloqueado* personaje_victima = list_get(personajes_deadlock, 0);

		//Se informa por archivo de log
		logger_info(get_logger(nivel), "DEADLOCK: El personaje %s ha sido seleccionado como victima del deadlock", personaje_victima->nombre);

		//aca muere el personaje seleccionado
		muerte_del_personaje(personaje_victima->simbolo, nivel, DEADLOCK);
		nivel_gui_dibujar(nivel);
	}
	else
		logger_info(get_logger(nivel), "DEADLOCK: El recovery no se encuentra activado");
}


t_list* cargar_lista_personajes(tad_nivel* nivel){
	t_list* lista_personajes = list_create();

	//Se agregan los personajes del nivel a lista auxiliar lista_personajes
	mutex_close(nivel->semaforo_personajes);

	foreach(personaje_nivel, nivel->personajes, tad_personaje*){
		alloc(personaje, tad_personaje);
		personaje->nombre = personaje_nivel->nombre;
		personaje->simbolo = personaje_nivel->simbolo;
		personaje->pos = personaje_nivel->pos;

		alloc(recurso_pedido, tad_recurso);
		recurso_pedido->simbolo = personaje_nivel->recurso_pedido->simbolo;
		recurso_pedido->cantidad = personaje_nivel->recurso_pedido->cantidad;
		personaje->recurso_pedido = recurso_pedido;

		personaje->recursos_asignados = list_create();
		foreach(recurso_personaje, personaje_nivel->recursos_asignados, tad_recurso*){
			alloc(recurso_asignado, tad_recurso);
			recurso_asignado->simbolo = recurso_personaje->simbolo;
			recurso_asignado->cantidad = recurso_personaje->cantidad;
			list_add(personaje->recursos_asignados, recurso_asignado);
		}
		list_add(lista_personajes, personaje);
	}

	mutex_open(nivel->semaforo_personajes);

	return lista_personajes;
}

t_list* cargar_lista_recursos(tad_nivel* nivel){
	t_list* recursos_disponibles = list_create();

	//Se agregan los recursos del nivel a lista auxiliar recursos_disponible
	mutex_close(nivel->semaforo_cajas);

	foreach(caja, nivel->cajas, tad_caja*){
		alloc(recurso, tad_recurso);
		recurso->simbolo = caja->simbolo;
		recurso->cantidad = caja->instancias;
		list_add(recursos_disponibles, recurso);
	}

	mutex_open(nivel->semaforo_cajas);

	return recursos_disponibles;
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

	int flag_cambios = 0;

	logger_info(get_logger(nivel),
		"DEADLOCK: Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
		nivel->tiempo_deadlock / 10000.0);

	 t_list* personajes_deadlock = NULL;
     t_list* lista_personajes = NULL;
     t_list* recursos_disponibles = NULL;

	while(true){

		usleep(nivel->tiempo_deadlock);
		//sleep(1);

		personajes_deadlock = list_create();
		bool hay_bloqueados = false;

		lista_personajes = cargar_lista_personajes(nivel);

		if(!(list_is_empty(lista_personajes))){

			recursos_disponibles = cargar_lista_recursos(nivel);

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
			if(hay_bloqueados){

				do {
					flag_cambios = 0;

					foreach(personaje, lista_personajes, tad_personaje*){

						//logger_info(get_logger(nivel),"DEADLOCK: personaje %s bloqueado, cantidad de recursos asignados: %d", personaje->nombre, list_size(personaje->recursos_asignados));

						//Para los personajes bloqueados con recursos asignados se verifica si se puede otorgar el recurso solicitado
						if(list_size(personaje->recursos_asignados) > 0){

							//Se busca el recurso solicitado en la lista de recursos disponibles
							char recurso_aux = personaje->recurso_pedido->simbolo;

							//logger_info(get_logger(nivel),"DEADLOCK: Buscando recurso solicitado: %c", recurso_aux);

							int encontre_recurso(tad_recurso* caja_recurso){
								return (recurso_aux == caja_recurso->simbolo);
							}

							tad_recurso* caja_recurso= list_find(recursos_disponibles, (void*) encontre_recurso);

							//Si se puede otorgar el recurso solicitado se liberan los recursos asignados
							if(caja_recurso->cantidad > 0){

								//logger_info(get_logger(nivel),"DEADLOCK: Se encontro recurso %c, se puede otorgar instancia", caja_recurso->simbolo);
								liberar_recursos_del_personaje(personaje, recursos_disponibles);

								personaje->recurso_pedido->cantidad = 0;
								personaje->recurso_pedido->simbolo = '\0';
								flag_cambios = 1;
							}
							else{
								//logger_info(get_logger(nivel),"DEADLOCK: No se puede otorgar instancia del recurso", caja_recurso->simbolo);
							}
						}
					}
					//Si se liberaron recursos se vuelve a recorrer la lista para ver si se puede desbloquear algun personaje
				}while(flag_cambios != 0);


				char *str_personajes_deadlock = string_new();

				//logger_info(get_logger(nivel),"DEADLOCK: Controlo si hay deadlock");

				//Se dentifican los personajes en deadlock por tener recurso pedido (bloqueado) y recursos asignados y se cargan en una nueva lista
				foreach(personaje, lista_personajes, tad_personaje*){
					var(cant_recurso, personaje->recurso_pedido->cantidad);

					//logger_info(get_logger(nivel),"DEADLOCK: cant recursos asignados del personaje %c: %d", personaje->simbolo, list_size(personaje->recursos_asignados));
					if((cant_recurso == 1) && ((list_size(personaje->recursos_asignados)) > 0)){
						alloc(personaje_bloqueado, t_personaje_bloqueado);
						personaje_bloqueado->simbolo = personaje->simbolo;
						personaje_bloqueado->nombre = personaje->nombre;
						list_add(personajes_deadlock, personaje_bloqueado);
						string_append_with_format(&str_personajes_deadlock, "%s.", personaje_bloqueado->nombre);

						logger_info(get_logger(nivel),"DEADLOCK: Personaje bloqueado con recursos asignados: %s", personaje->nombre);
					}
				}

				//Si existen por lo menos dos personajes en la lista entonces hay deadlock
				if(list_size(personajes_deadlock) > 1){

					//Se informa deadlock por archivo log indicando personajes involucrados;
					logger_info(get_logger(nivel),
							"DEADLOCK: Se detecto deadlock. Personajes involucrados: %s", str_personajes_deadlock);
					resolver_deadlock(nivel, personajes_deadlock);
				}
                free(str_personajes_deadlock);
			}

			  list_destroy_and_destroy_elements(personajes_deadlock, (void*) liberar_bloqueado);
			//list_destroy(personajes_deadlock);
			liberar_lista_recursos(recursos_disponibles);
			//list_destroy(recursos_disponibles);
		}
		liberar_lista_personajes(lista_personajes);
	}
}


private void liberar_personaje(void* ptr_pj){
   if (ptr_pj == NULL) return;
   tad_personaje* pj = (tad_personaje*) ptr_pj;

   if(pj->recursos_asignados != NULL)
	   list_destroy_and_destroy_elements(pj->recursos_asignados, free);
	   //list_destroy(pj->recursos_asignados);
   if(pj->recurso_pedido != NULL)
     dealloc(pj->recurso_pedido);

   dealloc(pj);
 }


private void liberar_bloqueado(t_personaje_bloqueado* personaje){
	//free(personaje->nombre);
	free(personaje);
}


 private void liberar_lista_personajes(t_list* lista){
   if (lista == NULL) return;
   list_destroy_and_destroy_elements(lista, liberar_personaje);
 }


 private void liberar_lista_recursos(t_list* lista){
   if (lista == NULL) return;
   list_destroy_and_destroy_elements(lista, free);
 }


