#include "character.h"

t_character* character_create(char* character, char* color, int x, int y) {
	t_character* this = new(t_character);
	this->update = NULL;

	this->position.x = x;
	this->position.y = y;
	this->position.z = 0;

	this->text = character;
	this->colors_text = color;
	this->colors_back = "0";

	return this;
}
