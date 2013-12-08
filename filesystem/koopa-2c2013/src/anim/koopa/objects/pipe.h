#ifndef PIPE_H_
#define PIPE_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		char state;
		long lastUpdateTime;
	} t_pipe;

//	 __ < state: A =
/*	|__|             /\ */
//	|  |         B = ||
//	|__|
//                  \  /
//               C = ||
//
//
//               D = B

	t_pipe* pipe_create(int x, int y);
	t_pipe* pipe_thePlantDied(t_pipe* this);

#endif
