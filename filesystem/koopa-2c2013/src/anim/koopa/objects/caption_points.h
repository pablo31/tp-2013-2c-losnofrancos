#ifndef CAPTION_POINTS_H_
#define CAPTION_POINTS_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		long points;
		int coins;
	} t_caption_points;

//	MARIO
//	022600   o x 16

	t_caption_points* caption_points_create(int x, int y);

#endif
