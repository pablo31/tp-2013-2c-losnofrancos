#ifndef DUMMY_H_
#define DUMMY_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		int movements;
		bool right;
	} t_dummy;

	t_dummy* dummy_create();

#endif
