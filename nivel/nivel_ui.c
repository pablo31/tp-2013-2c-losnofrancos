#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <time.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/logger/logger.h"
#include "../libs/common/config.h"
#include "../libs/vector/vector2.h"
#include "../libs/common.h"

static WINDOW * secwin;
static WINDOW * mainwin;
static int rows, cols;
static int inicializado = 0;

static void nivel_gui_get_term_size(int * rows, int * cols) {
    struct winsize ws;

    if ( ioctl(0, TIOCGWINSZ, &ws) < 0 ) {
        perror("couldn't get window size");
    }

    *rows = ws.ws_row;
    *cols = ws.ws_col;
}
static int nivel_gui_int_validar_inicializado(void){
	return inicializado;
}
static void nivel_gui_print_perror(const char* message){
	fprintf(stderr, "%s\n", message);
}

int nivel_gui_inicializar() {	
	
	if (nivel_gui_int_validar_inicializado()){
		return EXIT_FAILURE;
	}

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

	inicializado = 1;

	return EXIT_SUCCESS;
}


int nivel_gui_dibujar(ITEM_NIVEL* items) {
	if (!nivel_gui_int_validar_inicializado()){
		nivel_gui_print_perror("nivel_gui_dibujar: Library no inicializada!");
		return EXIT_FAILURE;
	}

	if (items == NULL){
		nivel_gui_print_perror("nivel_gui_dibujar: La lista de items no puede ser NULL");
		return EXIT_FAILURE;
	}


	ITEM_NIVEL *temp = items;
	int i = 0;

	werase(secwin);
	box(secwin, 0, 0);
	wbkgd(secwin, COLOR_PAIR(1));

	move(rows - 2, 2);
	printw("Recursos: ");

	while (temp != NULL) {
		wmove (secwin, temp->posx, temp->posy);
		if (temp->item_type) {
			waddch(secwin, temp->id | COLOR_PAIR(3));
		} else {
			waddch(secwin, temp->id | COLOR_PAIR(2));
		}
		if (temp->item_type) {
			move(rows - 2, 7 * i + 3 + 9);
			printw("%c: %d - ", temp->id, temp->quantity);
			i++;
		}
		temp = temp->next;

	}
	wrefresh(secwin);
	wrefresh(mainwin);

	return EXIT_SUCCESS;
}

int nivel_gui_terminar() {
		if (!nivel_gui_int_validar_inicializado()){
			//tp_logger_error(logger,"nivel_gui_terminar: Library no inicializada!");
			return EXIT_FAILURE;
		}


		//tp_logger_debug(logger, "Borrando ventanas de Curses.");
        delwin(mainwin);
        delwin(secwin);
        endwin();
        refresh();

        return EXIT_SUCCESS;
}

int nivel_gui_get_area_nivel(int * rows, int * cols) {
	if (!nivel_gui_int_validar_inicializado()){
			nivel_gui_print_perror("nivel_gui_get_area_nivel: Library no inicializada!");
			return EXIT_FAILURE;
	}

	if (rows == NULL || cols == NULL){
		nivel_gui_print_perror("nivel_gui_get_area_nivel: Ninguno de los argumentos puede ser NULL");
		return EXIT_FAILURE;
	}

	nivel_gui_get_term_size(rows, cols);
	*rows = *rows - 4;
	*cols = *cols - 2;

	return EXIT_SUCCESS;
}

static void nivel_gui_crear_item(ITEM_NIVEL** ListaItems, char id, int x , int y, char tipo, int cant_rec) {
        ITEM_NIVEL * temp;
        temp = malloc(sizeof(ITEM_NIVEL));

        temp->id = id;
        temp->posx=x;
        temp->posy=y;
        temp->item_type = tipo;
        temp->quantity = cant_rec;
        temp->next = *ListaItems;
        *ListaItems = temp;
}

void nivel_gui_crear_personaje(ITEM_NIVEL** ListaItems, char id, int x , int y) {
        nivel_gui_crear_item(ListaItems, id, x, y, PERSONAJE_ITEM_TYPE, 0);
}

void nivel_gui_crear_caja(ITEM_NIVEL** ListaItems, tad_caja* c) {
        nivel_gui_crear_item(ListaItems, c->simbolo, c->pos.x, c->pos.y, RECURSO_ITEM_TYPE, c->instancias);
}


void nivel_gui_crear_enemigo(ITEM_NIVEL** ListaItems, tad_enemigo* enem, int seed) {
		//la seed o semilla es un numero para generar valores aleatorios. 
		//Se lo paso por argumento asi lo modifico en el for anterior, sino me da siempre numeros iguales
		// le sumo 1 al resultado porque no puede ser 0/0 la posicion.
		srand (seed);
		int pos_x = 1 + (rand() % rows);
		int pos_y = 1 + (rand() % cols);

		enem->pos = vector2_new(pos_x, pos_y);
		
        nivel_gui_crear_item(ListaItems, enem->simbolo, enem->pos.x, enem->pos.y, ENEMIGO_ITEM_TYPE, 1);
}

void nivel_borrar_item(ITEM_NIVEL** ListaItems, char id) {
        ITEM_NIVEL * temp = *ListaItems;
        ITEM_NIVEL * oldtemp;

        if ((temp != NULL) && (temp->id == id)) {
                *ListaItems = (*ListaItems)->next;
		free(temp);
        } else {
                while((temp != NULL) && (temp->id != id)) {
                        oldtemp = temp;
                        temp = temp->next;
                }
                if ((temp != NULL) && (temp->id == id)) {
                        oldtemp->next = temp->next;
			free(temp);
                }
        }

}

void nivel_gui_mover_personaje(ITEM_NIVEL* ListaItems, char id, int x, int y) {
        ITEM_NIVEL * temp;
        temp = ListaItems;

        while ((temp != NULL) && (temp->id != id)) {
                temp = temp->next;
        }
        if ((temp != NULL) && (temp->id == id)) {
                temp->posx = x;
                temp->posy = y;
        }
}

void nivel_restar_recurso(ITEM_NIVEL* ListaItems, char id) {
        ITEM_NIVEL * temp;
        temp = ListaItems;

        while ((temp != NULL) && (temp->id != id)) {
                temp = temp->next;
        }
        if ((temp != NULL) && (temp->id == id)) {
                if ((temp->item_type) && (temp->quantity > 0)) {
                		int quantity = temp->quantity;
                		quantity--;
                        temp->quantity = quantity;
                }
        }
}

void cargar_recursos_nivel(tad_nivel* nivel){	
	ITEM_NIVEL* ListaItems = NULL;

	
	//int q, p;

	//int x = 1;
	//int y = 1;

    //nivel_gui_get_area_nivel(&rows, &cols);

	//q = rows;
	//p = cols;

	//tp_logger_info(logger, "Cargando recursos del nivel en la libreria de Curses.");
	int cantidad_niveles = list_size(nivel->cajas); 
	int i;
	for (i=0; i < cantidad_niveles ; i++){
		tad_caja* caja = list_get(nivel->cajas,i);
		nivel_gui_crear_caja(&ListaItems, caja);
	}

	int cantidad_enemigos = list_size(nivel->enemigos);
	int seed = time(null);
	for (i=0; i < cantidad_enemigos; i++){
		tad_enemigo* enem = list_get(nivel->enemigos,i);
		nivel_gui_crear_enemigo(&ListaItems, enem, seed);
		seed++;
	}

	
	//tp_logger_info(logger, "Iniciando el dibujado del nivel.");
	nivel_gui_dibujar(ListaItems);
}
