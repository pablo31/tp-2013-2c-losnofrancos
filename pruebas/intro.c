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

    set rows = ws.ws_row;
    set cols = ws.ws_col;
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
	waddch(mainwin, c);
}



enum DIRECTIONS {
	UP = 0,
	DOWN = 1,
	RIGHT = 2,
	LEFT = 3
};
int direction;

vector2 win_bounds;


vector2 get_next_block(vector2 block, vector2 min, vector2 max, int as_out up_collision){
	set up_collision = 0;
	//horrible
	switch(direction){
	case UP:
		if(block.y == min.y + 1){
			set up_collision = 1;
			direction = RIGHT;
			return vector2_add_x(block, 1);
		}else{
			return vector2_add_y(block, -1);
		}
		break;
	case DOWN:
		if(block.y == max.y){
			direction = LEFT;
			return vector2_add_x(block, -1);
		}else{
			return vector2_add_y(block, 1);
		}
		break;
	case RIGHT:
		if(block.x == max.x){
			direction = DOWN;
			return vector2_add_y(block, 1);
		}else{
			return vector2_add_x(block, 1);
		}
		break;
	case LEFT:
		if(block.x == min.x){
			direction = UP;
			return vector2_add_y(block, -1);
		}else{
			return vector2_add_x(block, -1);
		}
		break;
	}
	return block;
}


void draw_block(vector2 screen_pos, int block_width, int block_height){
	int x; int y;
	for(x = 0; x < block_width; x++){
		for(y = 0; y < block_height; y++){
			gotoxy(screen_pos.x + x, screen_pos.y + y);
			add_char('0');
		}
	}
	wrefresh(mainwin);
}



void draw(){
	direction = RIGHT;

	int blocks = 20; //grid
	int total_time = 3; //seconds

	int height = win_bounds.y;
	int width = win_bounds.x;
	int block_height = height / blocks;
	int block_width = width / blocks;

	vector2 min = vector2_new(1, 1);
	vector2 max = vector2_new(blocks - 1, blocks - 1);
	vector2 block = min;
	int collision;

	int area = blocks * blocks;
	int u_total_time = total_time * 1000000;
	int interval = u_total_time / area;

	while(block.x >= min.x && block.x <= max.x && block.y >= min.y && block.y <= max.y){
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

//	gotoxy(0, 0);
//	wprintw(mainwin, "Cols: %d Rows: %d Interval: %d\n", win_bounds.x, win_bounds.y, interval);
//	wrefresh(mainwin);
//	sleep(2);
}


void print_on_center(const char* text){
	vector2 pos = vector2_divide(win_bounds, 2);
	pos.x -= strlen(text) / 2;
	gotopos(pos);
	wprintw(mainwin, text);
}



int main(){
	mainwin = initscr();
	start_color();
	noecho();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	werase(mainwin);

	win_bounds = gui_get_bounds();

	print_on_center("LosNoFrancos 2013");

	draw();

	delwin(mainwin);
	endwin();

	return EXIT_SUCCESS;
}
