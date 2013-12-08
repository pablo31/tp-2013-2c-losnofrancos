#include "goomba.h"

static void _goomba_update();

t_goomba* goomba_create(int x, int y) {
	t_goomba* this = new(t_goomba);
	this->update = (void*) _goomba_update;

	this->position.x = x;
	this->position.y = y;
	this->position.z = 0;

	this->text =        "-.-";
	this->colors_text = "444";
	this->colors_back = "000";

	this->movements = 0;

	return this;
}

void goomba_goRed(t_goomba* this) {
	this->colors_text = "111";
}

static void _goomba_update(i_drawable* _this) {
	downcast(t_goomba)

	if (this->right)
		this->position.x++;
	else
		this->position.x--;

	if (this->movements++ % 3 == 0)
		this->right = !this->right;
}
