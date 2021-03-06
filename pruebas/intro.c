/*
 * intro.c
 *
 *  Created on: Nov 3, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //usleep
#include <string.h>

#include <curses.h>
#include <sys/ioctl.h>

#include "../libs/vector/vector2.h"
#include "../libs/common.h"


static WINDOW * mainwin;


private void gui_get_size(int as_out rows, int as_out cols){
    struct winsize ws;

    if(ioctl(0, TIOCGWINSZ, &ws) < 0)
    	perror("couldn't get window size");

    set rows = ws.ws_row - 1;
    set cols = ws.ws_col - 1;
}
private vector2 gui_get_bounds(){
	int rows; int cols;
	gui_get_size(out rows, out cols);
	return vector2_new(cols, rows);
}

private void gotoxy(int x, int y){
	wmove(mainwin, y, x);
}
private void gotopos(vector2 pos){
	gotoxy(pos.x, pos.y);
}
private void add_char(char c){
	waddch(mainwin, c | COLOR_PAIR(2));
}



enum DIRECTIONS {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};
int direction;

vector2 win_bounds;



vector2 get_next_block(vector2 block, vector2 min, vector2 max, int as_out up_collision){
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


int wacum = 0;
int hacum = 0;
void draw_block(vector2 screen_pos, float block_width, float block_height){
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



void draw(){

	//user-def constants
	int blocks = 20; //grid
	int total_time = 3; //seconds

	//constants
	int height = win_bounds.y - 1;
	int width = win_bounds.x - 1;
	float block_height = (float)height / blocks;
	float block_width = (float)width / blocks;

	int area = blocks * blocks;
	int u_total_time = total_time * 1000000;
	int interval = u_total_time / area;

	//initial pos & direction
	direction = RIGHT;
	vector2 min = vector2_new(0, 0);
	vector2 max = vector2_new(blocks - 1, blocks - 1);
	vector2 block = min;
	int collision;


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


void print_on_center(const char* text){
	vector2 pos = vector2_divide(win_bounds, 2);
	pos.x -= strlen(text) / 2;
	gotopos(pos);
	wprintw(mainwin, text);
}



int main(){
	win_bounds = gui_get_bounds();

	mainwin = initscr();
	start_color();
	noecho();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	werase(mainwin);

	print_on_center("LosNoFrancos 2013");

	draw();

	delwin(mainwin);
	endwin();

	return EXIT_SUCCESS;
}
