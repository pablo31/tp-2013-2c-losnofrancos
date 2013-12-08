#include "textbox.h"

t_textbox* textbox_create(int x, int y, char* text) {
	t_textbox* this = new(t_textbox);
	this->update = NULL;

	this->position.x = x;
	this->position.y = y;
	this->position.z = 0;

	this->text = text;
	this->colors_text = string_repeat('7', strlen(text));
	this->colors_back = string_repeat('0', strlen(text));

	return this;
}
