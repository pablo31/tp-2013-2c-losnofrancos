#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <unistd.h>

#include "nivel.h"
#include "nivel_ui.h"

#include "../libs/logger/logger.h"
#include "../libs/vector/vector2.h"
#include "../libs/thread/mutex.h"
#include "../libs/common.h"


private WINDOW* secwin;
private WINDOW* mainwin;
private int rows, cols;
private tad_logger* logger;
private tad_mutex* semaforo_dibujado;


private void nivel_gui_get_term_size(int as_out rows, int as_out cols){
    struct winsize ws;

    if(ioctl(0, TIOCGWINSZ, &ws) < 0)
    	perror("couldn't get window size");

    set rows = ws.ws_row;
    set cols = ws.ws_col;
}

//void nivel_gui_get_area_nivel(int as_out rows, int as_out cols){
//	int term_rows;
//	int term_cols;
//	nivel_gui_get_term_size(out term_rows, out term_cols);
//	set rows = term_rows - 4;
//	set cols = term_cols - 2;
//}


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

	semaforo_dibujado = mutex_create();

	//TODO pokemon-style intro (see ../pruebas/intro.c)
}

private void nivel_gui_draw_char(char ch, vector2 pos, int color_pair){
	wmove(secwin, pos.y, pos.x);
	waddch(secwin, ch | COLOR_PAIR(color_pair));
}

void nivel_gui_dibujar(tad_nivel* nivel){
	int i = 0;

	mutex_close(semaforo_dibujado);

	//limpiamos la pantalla
	werase(secwin);
	box(secwin, 0, 0);
	wbkgd(secwin, COLOR_PAIR(1));

	move(rows - 2, 2);
	printw("Recursos: ");

	var(cajas, nivel->cajas);
	var(personajes, nivel->personajes);
	var(enemigos, nivel->enemigos);

	var(semaforo_cajas, nivel->semaforo_cajas);
	var(semaforo_enemigos, nivel->semaforo_enemigos);
	var(semaforo_personajes, nivel->semaforo_personajes);

	//dibujamos a los enemigos
	mutex_close(semaforo_enemigos);
	foreach(enemigo, enemigos, tad_enemigo*)
		nivel_gui_draw_char('*', enemigo->pos, 2);
	mutex_open(semaforo_enemigos);

	//dibujamos a los personajes
	mutex_close(semaforo_personajes);
	foreach(personaje, personajes, tad_personaje*)
		nivel_gui_draw_char(personaje->simbolo, personaje->pos, 2);
	mutex_open(semaforo_personajes);

	//dibujamos las cajas
	mutex_close(semaforo_cajas);
	foreach(caja, cajas, tad_caja*){
		nivel_gui_draw_char(caja->simbolo, caja->pos, 3);
		move(rows - 2, 7 * i + 3 + 9); //TODO algo mas lindo??
		printw("%c: %d - ", caja->simbolo, caja->instancias);
		i++;
	}
	mutex_open(semaforo_cajas);

	//actualizamos la pantalla
	wrefresh(secwin);
	wrefresh(mainwin);

	mutex_open(semaforo_dibujado);
}

void nivel_gui_terminar(){
	delwin(mainwin);
	delwin(secwin);
	endwin();
	refresh();
	mutex_dispose(semaforo_dibujado);
	logger_info(logger, "Finalizada");
	logger_dispose_instance(logger);
}
