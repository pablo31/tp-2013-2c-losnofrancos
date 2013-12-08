#include "koopa.h"

static void _koopa_doFocusHack(t_anim* anim);
static void _koopa_cleanObjects(t_anim* anim);
static void _koopa_createLineOfBlocks(t_anim* anim, int y);

static t_character *_mario, *_flower, *_coin1, *_coin2, *_shoot, *_hack;
static t_caption_points* _points;
static t_caption_status* _status;
static t_box* _box;
static t_goomba* _goomba;
static t_pipe* _pipe;
static t_bowser* _bowser;
static int floorLevel, lavaInit;
static long lastTime;
static char* state;

extern bool win;

void koopa_onInit(t_anim* anim) {
	lastTime = 0;
	state = "starting";

	//Floor and roof:
	_koopa_createLineOfBlocks(anim, 5);
	_koopa_createLineOfBlocks(anim, anim->screen->height - 1);
	floorLevel = anim->screen->height - 3;

	//Points:
	_points = caption_points_create(1, 3);
	anim_addToRenderList(anim, _points);

	//Status:
	_status = caption_status_create(3);
	_status->position.x = anim->screen->width - drawable_width(_status) - 3;
	anim_addToRenderList(anim, _status);

	//Pipe:
	_pipe = pipe_create(anim->screen->width * 0.75f, floorLevel);
	anim_addToRenderList(anim, _pipe);

	//Coins:
	_coin1 = character_create("o", "3", (int) (anim->screen->width * 0.4f), floorLevel);
	_coin2 = character_create("o", "3", (int) (anim->screen->width * 0.65f), floorLevel);
	anim_addToRenderList(anim, _coin1);
	anim_addToRenderList(anim, _coin2);

	//Box:
	_box = box_create((int) (anim->screen->width * 0.5f), floorLevel - 5);
	anim_addToRenderList(anim, _box);

	//Goomba:
	_goomba = goomba_create((int) (anim->screen->width * 0.2f), floorLevel);
	anim_addToRenderList(anim, _goomba);

	//Mario:
	_mario = character_create("@", "7", 1, floorLevel);
	anim_addToRenderList(anim, _mario);

	//Flower:
	_flower = character_create(" ", "5", _box->position.x + 2, _box->position.y - 1);
	anim_addToRenderList(anim, _flower);

	//Shoot:
	_shoot = character_create(" ", "1", 0, 0);
	anim_addToRenderList(anim, _shoot);

	//Hack for focus:
	_hack = character_create("_", "0", anim->screen->width - 3, 3);
	_hack->position.z = 999;
}

void koopa_onUpdate(t_anim* anim) {
	void _updateLastTime() { lastTime = drawable_getTime(); }
	long currentTime = drawable_getTime();
	long elapsedTime = currentTime - lastTime;

	if (string_equals(state, "starting")) {
		_mario->position.x++;

		if (_mario->position.x >= 6) {
			state = "waiting";
			_updateLastTime();
		}
	} else if (string_equals(state, "waiting")) {
		if (elapsedTime >= 1500) {
			state = "goToGoomba";
			_updateLastTime();
		}
	} else if (string_equals(state, "goToGoomba")) {
		_mario->position.x++;
		if (_goomba->position.x > _mario->position.x + 2 && _goomba->position.x < _mario->position.x + 5) {
			state = "jumping";
		}
	} else if (string_equals(state, "jumping")) {
		_mario->position.x++;
		_mario->position.y--;
		if (_mario->position.y < floorLevel - 2) {
			state = "aterrizanding"; //XD
		}
	} else if (string_equals(state, "aterrizanding")) {
		_mario->position.x++;
		_mario->position.y++;
		if (_mario->position.y == floorLevel) {
			goomba_goRed(_goomba);
			_points->points += 300;
			state = "goToCoin1";
			_updateLastTime();
		}
	} else if (string_equals(state, "goToCoin1")) {
		if (elapsedTime > 80) anim_removeFromRenderList(anim, _goomba);

		if (_mario->position.x == _coin1->position.x) {
			anim_removeFromRenderList(anim, _coin1);
			_points->coins++;
			_points->points += 200;
		}

		if (_mario->position.x == _coin2->position.x) {
			anim_removeFromRenderList(anim, _coin2);
			_points->coins++;
			_points->points += 200;
			_updateLastTime();
		}

		if (_mario->position.x == _pipe->position.x - 5) {
			if (elapsedTime >= 700)
				state = "forgotTheBox";
		} else _mario->position.x++;

	} else if (string_equals(state, "forgotTheBox")) {
		_mario->position.x--;
		if (_mario->position.x == _box->position.x + 2) {
			state = "jumpToBox";
		}
	} else if (string_equals(state, "jumpToBox")) {
		_mario->position.y--;
		if (_mario->position.y == _box->position.y + 1) {
			_mario->position.y++;
			box_clean(_box);
			_flower->text = "*";
			state = "aterrizandingFromBox";
			_updateLastTime();
		}
	} else if (string_equals(state, "aterrizandingFromBox")) {
		if (_mario->position.y != floorLevel)
			_mario->position.y++;

		_flower->position.x++;
		if (_flower->position.y >= _box->position.y - 3)
			_flower->position.y--;
		else
			state = "flowerFall";
	} else if (string_equals(state, "flowerFall")) {

		if (_flower->position.x < _pipe->position.x - 1)
			_flower->position.x++;
		else {
			_flower->position.x--;
			state = "flowerBounce";
		}

		if (_flower->position.y < floorLevel)
			_flower->position.y++;
	} else if (string_equals(state, "flowerBounce")) {
		_flower->position.x--;
		_mario->position.x++;

		if (_flower->position.x <= _mario->position.x) {
			anim_removeFromRenderList(anim, _flower);
			_mario->colors_text = "3";
			state = "jumpingThePipe";
			_updateLastTime();
		}
	} else if (string_equals(state, "jumpingThePipe")) {
		_mario->position.x++;
		if (_mario->position.x >= _pipe->position.x - 5)
			_mario->position.y--;

		if (_mario->position.y == _pipe->position.y - drawable_height(_pipe) + 2) {
			if (win) {
				_shoot->text = "-";
				_shoot->position.x = _mario->position.x + 1;
				_shoot->position.y = _mario->position.y;
				state = "shootingPlant";
			} else {
				state = "lose";
			}
		}
	} else if (string_equals(state, "lose")) {
		if (_mario->position.x != _pipe->position.x + 2) {
			_mario->position.x++;
			_mario->position.y--;
		} else {
			_mario->position.y++;
			if (_mario->position.y >= _pipe->position.y - drawable_height(_pipe) + 1) {
				anim_removeFromRenderList(anim, _mario);
				char* message = "Lo sentimos, pero la princesa esta en otro disco";
				t_textbox* textbox = textbox_create(
					anim->screen->width / 2 - strlen(message) / 2,
					anim->screen->height / 2,
					message
				);
				anim_addToRenderList(anim, textbox);
				state = "end";
				_updateLastTime();
			}
		}
	} else if (string_equals(state, "shootingPlant")) {
		if (_pipe->state != 'Z') {
			_shoot->position.x++;
			if (_shoot->position.x == _pipe->position.x + 2) {
				anim_removeFromRenderList(anim, _shoot);
				pipe_thePlantDied(_pipe);
			}
		}

		_mario->position.x++;
		_mario->position.y--;
		if (_pipe->state == 'Z' && _mario->position.y <= _pipe->position.y - drawable_height(_pipe))
			state = "goToKoopa";

	} else if (string_equals(state, "goToKoopa")) {
		if (_mario->position.y < floorLevel)
			_mario->position.y++;

		_mario->position.x++;
		if (_mario->position.x == anim->screen->width - 4) {
			_koopa_cleanObjects(anim);

			_mario->position.x = 1;
			anim_addToRenderList(anim, _mario);

			lavaInit = anim->screen->width / 2 - 8;
			void _makeLava(t_block* object) {
				if (object->position.y > floorLevel && object->position.x >= lavaInit) {
					block_makeLava((object));
				}
			}
			list_iterate(anim->objects, (void*) _makeLava);

			//Bowser:
			_bowser = bowser_create((int) (anim->screen->width * 0.6f), floorLevel, floorLevel - 5);
			anim_addToRenderList(anim, _bowser);

			state = "koopa'sPlace";
		}
	} else if (string_equals(state, "koopa'sPlace")) {
		_mario->position.x++;
		if (_mario->position.x >= 5)
			state = "koopattack";
	} else if (string_equals(state, "koopattack")) {
		_bowser->position.x--;

		if (_bowser->position.x <= anim->screen->width / 2 + 2) {
			free(_box);
			_box = box_create(_mario->position.x + 3, _mario->position.y - 4);
			anim_addToRenderList(anim, _box);
			state = "activateRain";
			_updateLastTime();
		}
	} else if (string_equals(state, "activateRain")) {
		if (elapsedTime > 500) {
			if (_mario->position.x >= _box->position.x + 2) {
				if (_mario->position.y <= _box->position.y + 1) {
					box_clean(_box);
					void _breakBridge(t_block* object) {
						if (object->position.y > floorLevel && object->position.x >= lavaInit) {
							block_breakBridge(object);
						}
					}
					list_iterate(anim->objects, (void*) _breakBridge);
					state = "fall";
					_updateLastTime();
				} else _mario->position.y--;
			} else _mario->position.x++;
		}
	} else if (string_equals(state, "fall")) {
		if (_mario->position.y != floorLevel)
			_mario->position.y++;

		if (elapsedTime > 300) {
			_updateLastTime();
			if (!strlen(_bowser->text)) {
				char* message = "Lo sentimos, pero no sabemos donde esta la princesa";
				t_textbox* textbox = textbox_create(
					anim->screen->width / 2 - strlen(message) / 2,
					anim->screen->height / 2,
					message
				);
				anim_addToRenderList(anim, textbox);
				state = "end";
				_updateLastTime();
			} else {
				bowser_killLastLine(_bowser);
			}
		}
	} else if (string_equals(state, "end")) {
		if (elapsedTime > 15000)
			anim->stop = true;
	}

	_koopa_doFocusHack(anim);
}

static void _koopa_doFocusHack(t_anim* anim) {
	anim_removeFromRenderList(anim, _hack);
	anim_addToRenderList(anim, _hack);
}

static void _koopa_cleanObjects(t_anim* anim) {
	bool _isAtTop(i_drawable* item) {
		return item->position.z != 0;
	}
	t_list* newItems = list_filter(anim->objects, (void*) _isAtTop);
	list_destroy(anim->objects);
	anim->objects = newItems;
}

static void _koopa_createLineOfBlocks(t_anim* anim, int y) {
	int next_x = 1;
	t_block* block;
	do {
		block = block_create();
		block->position.x = next_x;
		block->position.y = y;
		anim_addToRenderList(anim, block);
		next_x = block->position.x + drawable_width(block);
	} while(next_x < anim->screen->width - 1 - drawable_width(block));
}
