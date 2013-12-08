#ifndef BOX_H_
#define BOX_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		bool on;
		long lastUpdateTime;
	} t_box;

//	 ___
//	|_?_|

	t_box* box_create(int x, int y);
	void box_clean(t_box* this);

#endif
