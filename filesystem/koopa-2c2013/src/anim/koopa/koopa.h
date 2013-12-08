#ifndef KOOPA_H_
#define KOOPA_H_

	#include "../anim.h"
	#include "objects/block.h"
	#include "objects/caption_points.h"
	#include "objects/caption_status.h"
	#include "objects/pipe.h"
	#include "objects/character.h"
	#include "objects/box.h"
	#include "objects/goomba.h"
	#include "objects/bowser.h"
	#include "objects/textbox.h"

	void koopa_onInit(t_anim* anim);
	void koopa_onUpdate(t_anim* anim);

#endif

