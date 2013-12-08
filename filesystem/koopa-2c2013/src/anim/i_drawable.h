#ifndef DRAWABLE_H_
#define DRAWABLE_H_

	#include <sys/time.h>
	#include "headers.h"

	typedef struct {
		int x, y, z;
	} t_position;

	#define implements_i_drawable \
		void (*update)(void*); \
		char* text; \
		char* colors_text; \
		char* colors_back; \
		t_position position;

	typedef struct {
		implements_i_drawable
	} i_drawable;

	int drawable_width(void* drawable);
	int drawable_height(void* drawable);
	long drawable_getTime();

	//Colors:
	#define COL_BLACK 0
	#define COL_RED 1
	#define COL_GREEN 2
	#define COL_ORANGE 3
	#define COL_BLUE 4
	#define COL_GREY 5
	#define COL_SKYBLUE 6
	#define COL_WHIE 7

#endif
