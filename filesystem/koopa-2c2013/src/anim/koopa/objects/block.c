#include "block.h"

t_block* block_create() {
	t_block* this = new(t_block);
	this->update = NULL;

	this->position.x = 0;
	this->position.y = 0;
	this->position.z = 999;

	this->text =        "~__~\n|_||\n||_|";
	this->colors_text = "7777\n0000\n0000";
	this->colors_back = "0000\n6666\n6666";

	return this;
}

void block_makeLava(t_block* this) {
	this->text =        "----\n====\n----";
	this->colors_text = "0000\n0000\n1111";
	this->colors_back = "0000\n1111\n1111";
}

void block_breakBridge(t_block* this) {
	this->text =        "----\n----\n----";
	this->colors_text = "0000\n1111\n1111";
}
