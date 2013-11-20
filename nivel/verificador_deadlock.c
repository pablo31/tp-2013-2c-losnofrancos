#include <unistd.h>
#include "../libs/common/collections/list.h"
#include "../libs/common/string.h"
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


void verificador_deadlock(tad_nivel* nivel) {

	bool hay_deadlock;
	int flag_cambios = 0;

	t_list* personajes_deadlock = list_create();
	t_personaje_bloqueado* personaje_bloqueado;
	t_list *lista_personajes;
	t_list *recursos_disponibles = list_create();
	tad_recurso* recurso;
	tad_recurso* caja_recurso;


		logger_info(get_logger(nivel),
			"Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
			nivel->tiempo_deadlock / 10000.0);


	while (true) {

		sleep(nivel->tiempo_deadlock);

		lista_personajes = NULL;
		hay_deadlock = false;
		bool hay_bloqueados = false;

		*lista_personajes = *nivel->personajes;

		foreach (caja, nivel->cajas, tad_caja*){
			recurso->simbolo = caja->simbolo;
			recurso->cantidad = caja->instancias;
			list_add (recursos_disponibles, recurso);
		}


		//liberar recursos de los personajes que no estan bloqueados
		foreach(personaje, lista_personajes, tad_personaje*){
			if (personaje->recurso_pedido == '\0' && personaje->recursos_asignados != NULL){
				liberar_recursos_del_personaje(personaje, recursos_disponibles);
			}
			if (personaje->recurso_pedido != '\0'){
				hay_bloqueados = true;
			}
		}


		if (hay_bloqueados) {
			do {
				flag_cambios = 0;
				//verifico si puedo otorgar el recurso solicitado a aquellos personajes bloqueados
				foreach(personaje, lista_personajes, tad_personaje*){
					if (personaje->recurso_pedido != '\0'
							&& personaje->recursos_asignados != NULL) {

						tad_recurso* recurso_pedido_aux;
						recurso_pedido_aux->simbolo = personaje->recurso_pedido;
						recurso_pedido_aux->cantidad = 1;

						tad_recurso* recurso_p = recurso_pedido_aux;

						int encontre_recurso (tad_recurso* caja_recurso){
							if (recurso_p->simbolo == caja_recurso->simbolo){
								logger_info(get_logger(nivel), "El nivel %c encontro recurso",nivel->nombre);
								return 1;// TODO cambiar esto por un mensaje
							}
							else{
								logger_info(get_logger(nivel), "El nivel %c no encontro recurso",nivel->nombre);
								return 0; //TODO no hacer nada
							}

						}

						caja_recurso= list_find(recursos_disponibles, (void*) encontre_recurso);

						//si puedo otorgar el recurso solicitado libero los recursos asignados
						if(caja_recurso->cantidad > 0) {
							liberar_recursos_del_personaje(personaje, recursos_disponibles);
							personaje->recurso_pedido = '\0';
							flag_cambios = 1;
						}
					}
				}
			} while (flag_cambios != 0);


			//Identifico los personajes en deadlock por tener recurso pedido (bloqueado) y recursos asignados.
			foreach (personaje, lista_personajes, tad_personaje*){
				if (personaje->recurso_pedido != '\0'
						&& personaje->recursos_asignados != NULL) {
					personaje_bloqueado->simbolo = personaje->simbolo;
					personaje_bloqueado->nombre = personaje->nombre;
					list_add (personajes_deadlock, personaje_bloqueado);
					//string_append(personajes_deadlock, personaje_bloqueado);
					}
			}

			//si existen por lo menos dos personajes se informa deadlock
			if (list_size(personajes_deadlock) > 1)
				hay_deadlock = true;

			if (hay_deadlock){
				//informar deadlock por archivo log indicando personajes involucrados;
				  logger_info(nivel->logger, "Se detecto deadlock. Personajes: %s",personajes_deadlock);

				//si el recovery esta activado elijo personaje victima e informo al planificador
				if (nivel->recovery == 1) {
					//decido a que personaje matar
					//informo por archivo de log
					//informar_deadlock_al_planificador
				}
			}
		}
	}
	//return EXIT_FAILURE;   Jorge: pase la funcion de int a void, porque nivel.c no hace nada...
	// la responsabilidad de mandar mensajes, tiene que estar en la logica de verificador de deadlock.c
}
