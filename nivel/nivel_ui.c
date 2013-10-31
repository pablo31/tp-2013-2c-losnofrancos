#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <time.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/logger/logger.h"

static WINDOW * secwin;
static WINDOW * mainwin;
static int rows, cols;
private t_list* items; //list<gui_item>


private void nivel_gui_get_term_size(int as_out rows, int as_out cols){
    struct winsize ws;

    if(ioctl(0, TIOCGWINSZ, &ws) < 0)
    	perror("couldn't get window size");

    set rows = ws.ws_row;
    set cols = ws.ws_col;
}

//private void nivel_gui_get_area_nivel(int as_out rows, int as_out cols){
//	int term_rows;
//	int term_cols;
//	nivel_gui_get_term_size(out term_rows, out term_cols);
//	set rows = term_rows - 4;
//	set cols = term_cols - 2;
//}


void nivel_gui_inicializar(){
	mainwin = initscr();
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	init_pair(2,COLOR_WHITE, COLOR_BLACK);
	init_pair(3,COLOR_BLACK, COLOR_YELLOW);
	box(stdscr, 0, 0);
	refresh();

	nivel_gui_get_term_size(&rows, &cols);
	secwin = newwin(rows - 2, cols, 0, 0);
	box(secwin, 0, 0);
	wrefresh(secwin);

	items = list_create();
}


void nivel_gui_dibujar() {
	int i = 0;

	werase(secwin);
	box(secwin, 0, 0);
	wbkgd(secwin, COLOR_PAIR(1));

	move(rows - 2, 2);
	printw("Recursos: ");

	foreach(item, items, gui_item*){
		wmove (secwin, item->pos.x, item->pos.y);
		if (item->item_type) {
			waddch(secwin, item->id | COLOR_PAIR(3));
		} else {
			waddch(secwin, item->id | COLOR_PAIR(2));
		}
		if (item->item_type) {
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
}

static void nivel_gui_crear_item(char id, vector2 pos, char tipo, int cant_rec) {
        alloc(item, gui_item);

        item->id = id;
        item->pos = pos;
        item->item_type = tipo;
        item->quantity = cant_rec;

        list_add(items, item);
}

void nivel_gui_crear_personaje(char id, vector2 pos) {
	nivel_gui_crear_item(id, pos, PERSONAJE_ITEM_TYPE, 0);
}

void nivel_gui_crear_caja(tad_caja* c) {
	nivel_gui_crear_item(c->simbolo, c->pos, RECURSO_ITEM_TYPE, c->instancias);
}

void nivel_gui_crear_enemigo(tad_enemigo* enem, int seed) {
		//la seed o semilla es un numero para generar valores aleatorios. 
		//Se lo paso por argumento asi lo modifico en el for anterior, sino me da siempre numeros iguales
		// le sumo 1 al resultado porque no puede ser 0/0 la posicion.
		srand (seed);
		int x = 1 + (rand() % rows);
		int y = 1 + (rand() % cols);

		enem->pos = vector2_new(x, y);
		
        nivel_gui_crear_item(enem->simbolo, enem->pos, ENEMIGO_ITEM_TYPE, 1);
}

void nivel_borrar_item(char id) {
	bool item_buscado(void* ptr_item){
		return ((gui_item*)ptr_item)->id == id;
	}
	list_remove_by_condition(items, item_buscado);
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
	foreach(caja, nivel->cajas, tad_caja*){
		nivel_gui_crear_caja(caja);
	}

	int seed = time(null);
	foreach(enemigo, nivel->enemigos, tad_enemigo*){
		nivel_gui_crear_enemigo(enemigo, seed);
		seed++;
	}
	
	nivel_gui_dibujar();
}
