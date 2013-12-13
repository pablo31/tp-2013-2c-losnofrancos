#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

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
private int gui_inicializada = 0;


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



/***************************************************************
 * INTRO
 ***************************************************************/

enum DIRECTIONS {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};
private int direction;

private void gotoxy(int x, int y){
	wmove(mainwin, y, x);
}
private void add_char(char c){
	waddch(mainwin, c | COLOR_PAIR(4));
}

private vector2 get_next_block(vector2 block, vector2 min, vector2 max, int as_out up_collision){
	set up_collision = 0;

	vector2 next_block = block;

	direction--;

	do{
		direction++;
		if(direction > 3) direction = 0;

		vector2 movement;

		switch(direction){
		case UP: movement = vector2_new(0, -1); break;
		case RIGHT: movement = vector2_new(1, 0); break;
		case DOWN: movement = vector2_new(0, 1); break;
		case LEFT: movement = vector2_new(-1, 0); break;
		}

		next_block = vector2_add(block, movement);

	}while(!vector2_between_or_equals(next_block, min, max));

	if(vector2_equals(next_block, min)){
		next_block = vector2_new(min.x + 1, min.y + 1);
		set up_collision = 1;
	}

	return next_block;
}

private int wacum = 0;
private int hacum = 0;
private void draw_block(vector2 screen_pos, float block_width, float block_height){
	wacum = block_width - (int)block_width;
	int deltaw;
	if(wacum >= 1){
		deltaw = 1;
		wacum--;
	}else
		deltaw = 0;

	hacum = block_width - (int)block_width;
	int deltah;
	if(hacum >= 1){
		deltah = 1;
		hacum--;
	}else
		deltah = 0;

	int x; int y;
	for(x = 0; x < block_width + deltaw; x++){
		for(y = 0; y < block_height + deltah; y++){
			gotoxy(screen_pos.x + x, screen_pos.y + y);
			add_char(' ');
		}
	}
	wrefresh(mainwin);
}

private void nivel_gui_do_intro(){
	werase(mainwin);

	//user-def constants
	int blocks = 20; //grid
	int total_time = 2400; //miliseconds

	//constants
	vector2 win_bounds = vector2_new(cols, rows);
	int width = win_bounds.x - 1;
	int height = win_bounds.y - 1;
	float block_width = (float)width / blocks;
	float block_height = (float)height / blocks;

	int area = blocks * blocks;
	int u_total_time = total_time * 1000;
	int interval = u_total_time / area;

	//initial pos & direction
	direction = RIGHT;
	vector2 min = vector2_new(0, 0);
	vector2 max = vector2_new(blocks - 1, blocks - 1);
	vector2 block = min;
	int collision;

	//imprimimos un texto en el centro
	vector2 pos = vector2_divide(win_bounds, 2);
	char* text = "LosNoFrancos 2013";
	pos.x -= strlen(text) / 2;
	gotoxy(pos.x, pos.y);
	wprintw(mainwin, text);


	while(vector2_between_or_equals(block, min, max)){
		vector2 real_pos;
		real_pos.x = block_width * block.x;
		real_pos.y = block_height * block.y;
		draw_block(real_pos, block_width, block_height);

		usleep(interval);

		block = get_next_block(block, min, max, out collision);
		if(collision){
			min.x++;
			min.y++;
			max.x--;
			max.y--;
		}
	}
}




/***************************************************************
 * LOGICA
 ***************************************************************/


void nivel_gui_inicializar(){
	logger = logger_new_instance("GUI");

	mainwin = initscr();
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	init_pair(2,COLOR_WHITE, COLOR_BLACK);
	init_pair(3,COLOR_BLACK, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_RED);

	nivel_gui_get_term_size(out rows, out cols);

	nivel_gui_do_intro();

	werase(mainwin);
	box(stdscr, 0, 0);
	refresh();

	logger_info(logger, "Consola de %d filas y %d columnas", rows, cols);

	secwin = newwin(rows - 2, cols, 0, 0);
	box(secwin, 0, 0);
	wrefresh(secwin);

	logger_info(logger, "Inicializada");

	semaforo_dibujado = mutex_create();

	gui_inicializada = 1;
}

private void nivel_gui_draw_char(char ch, vector2 pos, int color_pair){
	wmove(secwin, pos.y, pos.x);
	waddch(secwin, ch | COLOR_PAIR(color_pair));
}

void nivel_gui_dibujar(tad_nivel* nivel){
	int i = 0;
	vector2 offset = vector2_new(1, 1);

	var(semaforo_cajas, nivel->semaforo_cajas);
	var(semaforo_enemigos, nivel->semaforo_enemigos);
	var(semaforo_personajes, nivel->semaforo_personajes);

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

	mutex_close(semaforo_personajes);
	mutex_close(semaforo_enemigos);
	mutex_close(semaforo_cajas);


	//dibujamos a los enemigos
	foreach(enemigo, enemigos, tad_enemigo*)
		nivel_gui_draw_char('*', vector2_add(offset, enemigo->pos), 2);

	//dibujamos a los personajes
	foreach(personaje, personajes, tad_personaje*)
		nivel_gui_draw_char(personaje->simbolo, vector2_add(offset, personaje->pos), 2);

	//dibujamos las cajas
	foreach(caja, cajas, tad_caja*){
		nivel_gui_draw_char(caja->simbolo, vector2_add(offset, caja->pos), 3);
		move(rows - 2, 7 * i + 3 + 9); //TODO algo mas lindo??
		printw("%c: %d - ", caja->simbolo, caja->instancias);
		i++;
	}

	mutex_open(semaforo_personajes);
	mutex_open(semaforo_enemigos);
	mutex_open(semaforo_cajas);

	//actualizamos la pantalla
	wrefresh(secwin);
	wrefresh(mainwin);

	mutex_open(semaforo_dibujado);
}

void nivel_gui_terminar(){
	if(!gui_inicializada) return;

	delwin(mainwin);
	delwin(secwin);
	endwin();
	refresh();
	mutex_dispose(semaforo_dibujado);
	logger_info(logger, "Finalizada");
	logger_dispose_instance(logger);
	gui_inicializada = 0;
}
