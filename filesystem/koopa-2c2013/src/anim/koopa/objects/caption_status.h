#ifndef CAPTION_STATUS_H_
#define CAPTION_STATUS_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
		int time;
		long lastUpdateTime;
	} t_caption_status;

//	WORLD   TIME
//	 8-8     373

	t_caption_status* caption_status_create(int y);

#endif
