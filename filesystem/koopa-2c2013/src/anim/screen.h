#ifndef SCREEN_H_
#define SCREEN_H_

	#include <curses.h>
 	#include <sys/ioctl.h>
	#include "headers.h"
	#include "i_drawable.h"

	typedef struct {
		int width;
		int height;
		WINDOW* mainWin;
		WINDOW* secWin;
		t_list* colorPairs;
		int colorCount;
	} t_screen;

	typedef struct {
		int id;
		int text;
		int back;
	} t_color_pair;

	t_screen* screen_create();
	void screen_preRefresh(t_screen* screen);
	void screen_renderItem(t_screen* screen, i_drawable* item);
	void screen_refresh(t_screen* screen);
	void screen_destroy(t_screen* screen);

#endif
