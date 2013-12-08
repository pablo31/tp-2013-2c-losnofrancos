#ifndef CHARACTER_H_
#define CHARACTER_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
	} t_character;

//	@

	t_character* character_create();

#endif
