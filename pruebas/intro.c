/*
 * intro.c
 *
 *  Created on: Nov 3, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //usleep

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

vector2 win_pos;
vector2 win_bounds;


vector2 get_next_pos(vector2 pos, vector2 min, vector2 max, int as_out up_collision){
	set up_collision = 0;
	//horrible
	switch(direction){
	case UP:
		if(pos.y == min.y + 1){
			set up_collision = 1;
			direction = RIGHT;
			return vector2_add_x(pos, 1);
		}else{
			return vector2_add_y(pos, -1);
		}
		break;
	case DOWN:
		if(pos.y == max.y){
			direction = LEFT;
			return vector2_add_x(pos, -1);
		}else{
			return vector2_add_y(pos, 1);
		}
		break;
	case RIGHT:
		if(pos.x == max.x){
			direction = DOWN;
			return vector2_add_y(pos, 1);
		}else{
			return vector2_add_x(pos, 1);
		}
		break;
	case LEFT:
		if(pos.x == min.x){
			direction = UP;
			return vector2_add_y(pos, -1);
		}else{
			return vector2_add_x(pos, -1);
		}
		break;
	}
	return pos;
}

void draw(){
	win_pos = vector2_new(0, 0);
	direction = RIGHT;

	vector2 min = vector2_new(0, 0);
	vector2 max = win_bounds;
	int collision;

	int area = win_bounds.x * win_bounds.y;
	int total_time = 3500000;
	int elapsed_time = 0;
	int interval = total_time / area;

	int color = 1;

	while(1){
		if(win_pos.x > max.x || win_pos.x < min.x || win_pos.y > max.y || win_pos.y < min.y) break;

		gotopos(win_pos);
		add_char('O');
		wrefresh(mainwin);

		usleep(interval);
		elapsed_time += interval;

		if(total_time / 15 < elapsed_time){
			wbkgd(mainwin, COLOR_PAIR(color++));
			if(color == 3) color = 1;
			elapsed_time = 0;
		}

		win_pos = get_next_pos(win_pos, min, max, out collision);
		if(collision){
			min.x++;
			min.y++;
			max.x--;
			max.y--;
		}
	}

	gotoxy(0, 0);
	wprintw(mainwin, "Cols: %d Rows: %d Interval: %d\n", win_bounds.x, win_bounds.y, interval);
	wrefresh(mainwin);
	sleep(2);
}




int main(){
	mainwin = initscr();
	start_color();
	noecho();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	werase(mainwin);

	win_bounds = gui_get_bounds();

//	int x;
//	int y;

//	for(x = 0; x < win_bounds.x; x++){
//		for(y = 0; y < win_bounds.y; y++){
//			gotoxy(x, y);
//			add_char('a');
//			wrefresh(mainwin);
//			usleep(500);
//		}
//	}
	draw();

	delwin(mainwin);
	endwin();

	return EXIT_SUCCESS;
}
