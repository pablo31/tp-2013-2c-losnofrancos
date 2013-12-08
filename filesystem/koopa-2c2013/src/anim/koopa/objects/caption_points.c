#include "caption_points.h"

static void _caption_points_update(i_drawable* _this);

t_caption_points* caption_points_create(int x, int y) {
	t_caption_points* this = new(t_caption_points);
	this->update = (void*) _caption_points_update;

	this->position.x = x;
	this->position.y = y;
	this->position.z = 999;

	char* format =      "MARIO\n022600~~~o~x~16";
	this->colors_text = "77777\n777777777377777";
	this->colors_back = "00000\n000000000000000";
	this->text = string_duplicate(format);

	this->points = 22600;
	this->coins = 16;

	return this;
}

static void _caption_points_update(i_drawable* _this) {
	downcast(t_caption_points)

	char* pointsToUpdate = this->text + strlen(this->text) - 15;
	char newPoints[7];
	sprintf(newPoints, "%06ld", this->points);
	memcpy(pointsToUpdate, newPoints, 6);

	char* coinsToUpdate = this->text + strlen(this->text) - 2;
	char newCoins[3];
	sprintf(newCoins, "%02d", this->coins);
	strcpy(coinsToUpdate, newCoins);
}
