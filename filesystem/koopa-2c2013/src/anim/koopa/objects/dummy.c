#include "dummy.h"

static void _dummy_update();

t_dummy* dummy_create() {
	t_dummy* this = new(t_dummy);
	this->update = (void*) _dummy_update;

	this->position.x = 7;
	this->position.y = 5;
	this->position.z = 0;

	this->text =        "~___\n~~~~\n|~~|\n|__|";
	this->colors_text = "0123\n4567\n0123\n4567";
	this->colors_back = "0123\n4567\n0123\n4567";

	this->movements = 0;

	return this;
}

static void _dummy_update(i_drawable* _this) {
	downcast(t_dummy)

	if (this->right)
		this->position.x++;
	else
		this->position.x--;

	if (this->movements++ % 5 == 0)
		this->right = !this->right;

}
