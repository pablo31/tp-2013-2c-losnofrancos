#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <unistd.h> //usleep

#include "nivel.h"
#include "nivel_ui.h"

#include "../libs/logger/logger.h"
#include "../libs/common.h"
#include "../libs/vector/vector2.h"


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

	logger_info(logger, "Inicializada");

	//TODO pokemon-style intro (see ../pruebas/intro.c)
}


void nivel_gui_dibujar(tad_nivel* nivel){
	int i = 0;

	werase(secwin);
	box(secwin, 0, 0);
	wbkgd(secwin, COLOR_PAIR(1));

	move(rows - 2, 2);
	printw("Recursos: ");

	var(cajas, nivel->cajas);
	var(personajes, nivel->personajes);
	var(enemigos, nivel->enemigos);

	foreach(caja, cajas, tad_caja*){
		wmove(secwin, caja->pos.y, caja->pos.x);
		waddch(secwin, caja->simbolo | COLOR_PAIR(3));
		move(rows - 2, 7 * i + 3 + 9);
		printw("%c: %d - ", caja->simbolo, caja->instancias);
		i++;
	}

	foreach(enemigo, enemigos, tad_enemigo*){
		wmove(secwin, enemigo->pos.y, enemigo->pos.x);
		waddch(secwin, '*' | COLOR_PAIR(4));
	}

	foreach(personaje, personajes, tad_personaje*){
		wmove(secwin, personaje->pos.y, personaje->pos.x);
		waddch(secwin, personaje->simbolo | COLOR_PAIR(2));
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
