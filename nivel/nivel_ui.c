#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <unistd.h> //usleep
#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/logger/logger.h"


t_list* items; //list<gui_item>   Jorge, esto estaba el sabado y no rompe
private WINDOW * secwin;
private WINDOW * mainwin;
private int rows, cols;
private tad_logger* logger;


private void nivel_gui_get_term_size(int as_out rows, int as_out cols){
    struct winsize ws;

    if(ioctl(0, TIOCGWINSZ, &ws) < 0)
    	perror("couldn't get window size");

    set rows = ws.ws_row;
    set cols = ws.ws_col;
}

void nivel_gui_get_area_nivel(int as_out rows, int as_out cols){
	int term_rows;
	int term_cols;
	nivel_gui_get_term_size(out term_rows, out term_cols);
	set rows = term_rows - 4;
	set cols = term_cols - 2;
}


void nivel_gui_inicializar(){
	logger = logger_new_instance("GUI");

	mainwin = initscr();
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	init_pair(2,COLOR_WHITE, COLOR_BLACK);
	init_pair(3,COLOR_BLACK, COLOR_YELLOW);
	box(stdscr, 0, 0);
	refresh();

	nivel_gui_get_term_size(out rows, out cols);
	logger_info(logger, "Consola de %d filas y %d columnas", rows, cols);

	secwin = newwin(rows - 2, cols, 0, 0);
	box(secwin, 0, 0);
	wrefresh(secwin);

	items = list_create(); // TODO Jorge, de donde sale esto¿?

	logger_info(logger, "Inicializada");

	//TODO pokemon-style intro (see ../pruebas/intro.c)
}


void nivel_gui_dibujar() {
	int i = 0;

	werase(secwin);
	box(secwin, 0, 0);
	wbkgd(secwin, COLOR_PAIR(1));

	move(rows - 2, 2);
	printw("Recursos: ");




	foreach(item, items, gui_item*){
		wmove (secwin, item->pos.y, item->pos.x); //TODO coordenadas cruzadas, pero anda bien S:


		/*TODO
		 *
		 * El error porque no se mueven los enemigos esta aca...
		 *
		 *
		 * Segun la catedra:
		 *
		 * void _draw_element(ITEM_NIVEL* item) {
            wmove(secwin, item->posy, item->posx);
            if(item->item_type == ENEMIGO_ITEM_TYPE) {
                    waddch(secwin, '*' | COLOR_PAIR(4));
            } else if (item->item_type == RECURSO_ITEM_TYPE) {
                    waddch(secwin, item->id | COLOR_PAIR(3));
            } else if(item->item_type == PERSONAJE_ITEM_TYPE) {
                    waddch(secwin, item->id | COLOR_PAIR(2));
            }
            if (item->item_type == RECURSO_ITEM_TYPE) {
                move(rows - 2, 7 * i + 3 + 9);
                printw("%c: %d - ", item->id, item->quantity);
                i++;
            }
        }

        list_iterate(items, (void*) _draw_element);

        fin de la catedra......

		 *
		 *Lo nuestro, tendria que ser así

		 if(item->item_type == ENEMIGO_ITEM_TYPE)
                waddch(secwin, item->id | COLOR_PAIR(4))
		 else if (item->item_type== RECURSO_ITEM_TYPE)
				waddch(secwin, item->id | COLOR_PAIR(3));
		else if(item->item_type == PERSONAJE_ITEM_TYPE)
				waddch(secwin, item->id | COLOR_PAIR(2));

		 *
		 */

		if(item->item_type == ENEMIGO_ITEM_TYPE)
			waddch(secwin, item->id | COLOR_PAIR(4));
		else if(item->item_type == PERSONAJE_ITEM_TYPE)
			waddch(secwin, item->id | COLOR_PAIR(2));
		else if (item->item_type== RECURSO_ITEM_TYPE)
		{
			waddch(secwin, item->id | COLOR_PAIR(3));
			move(rows - 2, 7 * i + 3 + 9);
			printw("%c: %d - ", item->id, item->quantity);
			i++;
		}
	}

	wrefresh(secwin);
	wrefresh(mainwin);
}

void nivel_gui_terminar(){
	delwin(mainwin);
	delwin(secwin);
	endwin();
	refresh();
	logger_info(logger, "Finalizada");
	logger_dispose_instance(logger);
}

private void nivel_gui_crear_item(char id, vector2 pos, char tipo, int cantidad) {
	alloc(item, gui_item);

	item->id = id;
	item->pos = pos;
	item->item_type = tipo;
	item->quantity = cantidad;
	//item->recursos_asignados = string_new(); //item_personaje
	//item->recursos_asignados[0] = '\0'; //item_personaje

	list_add(items, item);

	logger_info(logger, "Agregado item %c", id);
}

void nivel_gui_crear_personaje(char simbolo, vector2 pos) {
	nivel_gui_crear_item(simbolo, pos, PERSONAJE_ITEM_TYPE, 0);
}

void nivel_gui_quitar_personaje(char simbolo){
	nivel_gui_borrar_item(simbolo);
}

void nivel_gui_crear_caja(tad_caja* c) {
	nivel_gui_crear_item(c->simbolo, c->pos, RECURSO_ITEM_TYPE, c->instancias);
}

void nivel_gui_crear_enemigo(tad_enemigo* enem) {
	nivel_gui_crear_item(enem->simbolo, enem->pos, ENEMIGO_ITEM_TYPE, 1);
}

void nivel_gui_move_enemigo(char simbolo, vector2 pos){
	nivel_gui_mover_item(simbolo,pos);
}

void nivel_gui_borrar_item(char id) {
	bool item_buscado(void* ptr_item){
		return ((gui_item*)ptr_item)->id == id;
	}
	list_remove_by_condition(items, item_buscado);
	logger_info(logger, "Borrado item %c", id);
}


void nivel_gui_mover_item(char id, vector2 new_pos){
	foreach(item, items, gui_item*)
		if(item->id == id)
			item->pos = new_pos;
}

void nivel_restar_recurso(char id){
	foreach(item, items, gui_item*)
		if(item->id == id)
			item->quantity--;
}

void cargar_recursos_nivel(tad_nivel* nivel){
	foreach(caja, nivel->cajas, tad_caja*)
		nivel_gui_crear_caja(caja);

	foreach(enemigo, nivel->enemigos, tad_enemigo*)
		nivel_gui_crear_enemigo(enemigo);
	
	nivel_gui_dibujar();
}

