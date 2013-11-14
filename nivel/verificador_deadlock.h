#ifndef VERIFICADOR_DEADLOCK_H_
#define VERIFICADOR_DEADLOCK_H_

#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/common/collections/list.h"


struct item {
	gui_item *item;
	struct item *next;
};
typedef struct item item_nivel;


void* verificador_deadlock(tad_nivel* nivel, t_list* items);

void liberar_recursos_del_personaje(item_nivel* personaje,	item_nivel* lista_items_control);

void liberar_lista(item_nivel *lista);

void informar_deadlock_al_planificador(tad_nivel* nivel,t_list* personajes_deadlock);

#endif
