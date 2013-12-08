#ifndef TEXTBOX_H_
#define TEXTBOX_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
	} t_textbox;

	t_textbox* textbox_create(int x, int y, char* text);

#endif
