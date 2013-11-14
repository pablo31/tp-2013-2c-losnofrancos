#include <unistd.h>
#include "../libs/common/collections/list.h"
#include "../libs/common/string.h"
#include "verificador_deadlock.h"


void liberar_recursos_del_personaje(item_nivel* personaje,
	item_nivel* lista_items_control) {
	item_nivel* item_recurso;
	int i = 0;
	while (personaje->item->recursos_asignados[i] != '\0') {
		item_recurso = lista_items_control;
		while (item_recurso != NULL && item_recurso->item->id != personaje->item->recursos_asignados[i]) {
			item_recurso = item_recurso->next;
		}
		item_recurso->item->quantity = item_recurso->item->quantity + personaje->item->recursos_asignados[i];
		i++;
	}
	personaje->item->recursos_asignados[0] = '\0';
}

/*
void liberar_lista(item_nivel *lista) {
	item_nivel *auxLista = NULL;
	while (*lista != NULL ) {
		auxLista = *lista;
		*lista = *lista->next;
		free(auxLista);
	}

}

void informar_deadlock_al_planificador(tad_nivel* nivel,t_list * personajes_deadlock){

}
*/

void *verificador_deadlock(tad_nivel* nivel, t_list* items) {

	item_nivel *lista_items_control;
	item_nivel *item_personaje;
	item_nivel *item_recurso;
	item_nivel *temp;

	bool hay_deadlock;
	int flag_cambios;
	int cont;

	char *personajes_deadlock = malloc(200);
	char *personaje_bloqueado = malloc(200);

	logger_info(get_logger(nivel),
			"Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
			nivel->tiempo_deadlock / 10000.0);


		while (true) {
		sleep(nivel->tiempo_deadlock);

		hay_deadlock = false;
		personajes_deadlock[0] = '\0';
		*lista_items_control = NULL;
		item_personaje = malloc(sizeof(item_nivel));
		item_personaje = items;

		while (item_personaje != NULL ) {
			temp = malloc(sizeof(item_nivel));
			//memcpy(temp, item_personaje, sizeof(item_nivel));
			temp = item_personaje;
			//temp->item->recursos_asignados = malloc(50);
			//strcpy(temp->item->recursos_asignados, item_personaje->item->recursos_asignados);
			temp->next = lista_items_control;
			lista_items_control = temp;
			item_personaje = item_personaje->next;
		}

		//liberar recursos de los personajes que no estan bloqueados
		cont = 0;
		item_personaje = lista_items_control;
		while (item_personaje != NULL ) {
			if (item_personaje->item->item_type == PERSONAJE_ITEM_TYPE) {
				cont++;
				if (item_personaje->item->recurso_pedido == '\0' && item_personaje->item->recursos_asignados[0] != '\0')
					liberar_recursos_del_personaje(item_personaje, lista_items_control);
			}
			item_personaje = item_personaje->next;
		}

		if (cont > 0) {

			do {
				flag_cambios = 0;
				item_personaje = lista_items_control;

				while (item_personaje != NULL ) {
					item_recurso = lista_items_control;

					if (item_personaje->item->item_type == PERSONAJE_ITEM_TYPE) {
						if (item_personaje->item->recurso_pedido != '\0'
								&& item_personaje->item->recursos_asignados[0] != '\0') {

							while ((item_recurso != NULL )&& (item_recurso->item->id != item_personaje->item->recurso_pedido)){
							item_recurso = item_recurso->next;
							}

							if(item_recurso->item->quantity > 0) {
								liberar_recursos_del_personaje(item_personaje, lista_items_control);
								item_personaje->item->recurso_pedido = '\0';
								flag_cambios = 1;
							}
						}
					}
					item_personaje = item_personaje->next;
				}
			} while (flag_cambios != 0);

			//Identifico los personajes en deadlock por tener recurso pedido (bloqueado) y recursos asignados.
			item_personaje = lista_items_control;
			while (item_personaje != NULL ) {
				if (item_personaje->item->item_type == PERSONAJE_ITEM_TYPE) {
					if (item_personaje->item->recurso_pedido != '\0'
							&& item_personaje->item->recursos_asignados[0] != '\0') {
						personaje_bloqueado[1] = '\0';
						personaje_bloqueado[0] = item_personaje->item->id;

						string_append(personajes_deadlock, personaje_bloqueado);
					}
				}
				item_personaje = item_personaje->next;
			}

			//si existen por lo menos dos personajes se informa deadlock
			if (string_length(personajes_deadlock) > 1)
				hay_deadlock = true;

			if (hay_deadlock){
				//informar deadlock por archivo log
				logger_info(nivel->logger, "Se detecto deadlock. Personajes: %s",personajes_deadlock);

				//si el recovery está activado informar al planificador cuales son los personajes involucrados
				if (nivel->recovery == 1) {

					//informar_deadlock_al_planificador(tad_nivel* nivel, t_list * personajes_deadlock);
				}
			}
		}
//		liberar_lista(lista_items_control);
	}

	free(personajes_deadlock);
	free(personaje_bloqueado);

	return NULL;
}
