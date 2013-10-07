#ifndef NIVEL_UI
#define NIVEL_UI

#define PERSONAJE_ITEM_TYPE 0
#define RECURSO_ITEM_TYPE 1
#define ENEMIGO_ITEM_TYPE 2

struct item {
	char id;
	int posx;
	int posy;
	char item_type; // PERSONAJE o CAJA_DE_RECURSOS
	int quantity;
	struct item *next;
};

typedef struct item ITEM_NIVEL;

/*
 *  Inicializacion y Destruccion del nivel
 */

int nivel_gui_inicializar();
int nivel_gui_terminar();
int nivel_gui_get_area_nivel(int * rows, int * cols);
int nivel_gui_dibujar(ITEM_NIVEL* items);

/*
 *  Acciones sobre recursos y personajes
 */
void nivel_borrar_item(ITEM_NIVEL** i, char id);
void nivel_restar_recurso(ITEM_NIVEL* i, char id);
void nivel_gui_mover_personaje(ITEM_NIVEL* i, char personaje, int x, int y);
void nivel_gui_crear_personaje(ITEM_NIVEL** i, char id, int x , int y);
void nivel_gui_crear_caja(ITEM_NIVEL** i, char id, int x , int y, int cant);
#endif