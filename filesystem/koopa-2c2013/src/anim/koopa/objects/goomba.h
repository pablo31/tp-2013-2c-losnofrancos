#ifndef GOOMBA_H_
#define GOOMBA_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		int movements;
		bool right;
	} t_goomba;

//	-.-

	t_goomba* goomba_create(int x, int y);
	void goomba_goRed(t_goomba* this);

#endif
