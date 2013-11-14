#ifndef NIVEL_UI
#define NIVEL_UI

#define PERSONAJE_ITEM_TYPE 0
#define RECURSO_ITEM_TYPE 1
#define ENEMIGO_ITEM_TYPE 2

#include "../libs/common.h"
#include "../libs/vector/vector2.h"

#include "nivel.h"


class(gui_item){
	char id;
	char item_type; //PERSONAJE o CAJA_DE_RECURSOS
	int quantity;
	vector2 pos;
	char recurso_pedido; //item_personaje
	char *recursos_asignados; //item_personaje
};


/*
 *  Inicializacion y Destruccion del nivel
 */

void nivel_gui_inicializar();
void nivel_gui_terminar();
void nivel_gui_dibujar();

/*
 *  Acciones sobre recursos y personajes
 */
void nivel_gui_borrar_item(char id);
void nivel_restar_recurso(char id);
void nivel_gui_mover_item(char id, vector2 new_pos);
void nivel_gui_crear_personaje(char simbolo, vector2 pos);
void nivel_gui_quitar_personaje(char simbolo);
void nivel_gui_crear_caja(tad_caja* c);
#endif
