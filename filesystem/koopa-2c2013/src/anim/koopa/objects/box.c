#include "box.h"

static void _box_update(i_drawable* _this);

t_box* box_create(int x, int y) {
	t_box* this = new(t_box);
	this->update = (void*)_box_update;

	this->position.x = x;
	this->position.y = y;
	this->position.z = 0;

	char* format =      "~___\n|_?_|";
	this->colors_text = "7777\n70007";
	this->colors_back = "0000\n03330";
	this->text = string_duplicate(format);

	this->on = false;
	this->lastUpdateTime = drawable_getTime();

	return this;
}

void box_clean(t_box* this) {
	string_replaceChar(this->text, '?', '_');
	this->update = NULL;
}

static void _box_update(i_drawable* _this) {
	downcast(t_box)

	long currentTime = drawable_getTime();
	if (currentTime - this->lastUpdateTime >= 1000) {
		if (this->on) {
			this->on = false;
			this->colors_text = "7777\n70007";
		} else {
			this->on = true;
			this->colors_text = "7777\n70707";
		}
		this->lastUpdateTime = currentTime;
	}
}
