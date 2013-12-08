#include "caption_status.h"

static void _caption_status_update(i_drawable* _this);

t_caption_status* caption_status_create(int y) {
	t_caption_status* this = new(t_caption_status);
	this->update = (void*) _caption_status_update;

	this->position.x = 0;
	this->position.y = y;
	this->position.z = 999;

	char* format =      "WORLD~~~TIME\n 8-8~~~~~373";
	this->colors_text = "777777777777\n777777777777";
	this->colors_back = "000000000000\n000000000000";
	this->text = string_duplicate(format);

	this->time = 373;
	this->lastUpdateTime = drawable_getTime();

	return this;
}

static void _caption_status_update(i_drawable* _this) {
	downcast(t_caption_status)

	char* timeToUpdate = this->text + strlen(this->text) - 3;
	char newTime[4];
	sprintf(newTime, "%03d", this->time);
	strcpy(timeToUpdate, newTime);

	long currentTime = drawable_getTime();
	if (currentTime - this->lastUpdateTime >= 1000) {
		this->time--;
		this->lastUpdateTime = currentTime;
	}
}
