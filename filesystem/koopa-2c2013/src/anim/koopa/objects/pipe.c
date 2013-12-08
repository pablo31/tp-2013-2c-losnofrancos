#include "pipe.h"

static void _pipe_update(i_drawable* _this);
static void _pipe_switchToStateA(t_pipe* this);
static void _pipe_switchToStateB(t_pipe* this);
static void _pipe_switchToStateC(t_pipe* this);
static void _pipe_switchToStateD(t_pipe* this);

t_pipe* pipe_create(int x, int y) {
	t_pipe* this = new(t_pipe);
	this->update = (void*) _pipe_update;
	this->position.x = x;
	this->position.y = y;
	this->position.z = 0;

	_pipe_switchToStateA(this);

	this->lastUpdateTime = drawable_getTime();

	return this;
}

t_pipe* pipe_thePlantDied(t_pipe* this) {
	_pipe_switchToStateA(this);
	this->state = 'Z';

	return this;
}

static void _pipe_update(i_drawable* _this) {
	downcast(t_pipe)

	long currentTime = drawable_getTime();

	switch (this->state) {
	case 'A':
		if (currentTime - this->lastUpdateTime >= 1200) {
			_pipe_switchToStateB(this);
			this->lastUpdateTime = currentTime;
		}
		break;
	case 'B':
		if (currentTime - this->lastUpdateTime >= 700) {
			_pipe_switchToStateC(this);
			this->lastUpdateTime = currentTime;
		}
		break;
	case 'C':
		if (currentTime - this->lastUpdateTime >= 300) {
			_pipe_switchToStateD(this);
			this->lastUpdateTime = currentTime;
		}
		break;
	case 'D':
		if (currentTime - this->lastUpdateTime >= 500) {
			_pipe_switchToStateA(this);
			this->lastUpdateTime = currentTime;
		}
	}
}

static void _pipe_switchToStateA(t_pipe* this) {
	this->text =        "~~__~\n~|__|\n~|~~|\n~|__|";
	this->colors_text = "22222\n22222\n22222\n22222";
	this->colors_back = "00000\n00000\n00000\n00000";
	this->state = 'A';
}

static void _pipe_switchToStateB(t_pipe* this) {
	this->text =        "~~/\\~\n~~||~\n~|__|~\n~|~~|~\n~|__|";
	this->colors_text = "11111\n11111\n222222\n222222\n22222";
	this->colors_back = "00000\n00000\n000000\n000000\n00000";
	this->state = 'B';
}

static void _pipe_switchToStateC(t_pipe* this) {
	this->text =        "~\\~~/\n~~||~\n~|__|~\n~|~~|~\n~|__|";
	this->colors_text = "11111\n11111\n222222\n222222\n22222";
	this->colors_back = "00000\n00000\n000000\n000000\n00000";
	this->state = 'C';
}

static void _pipe_switchToStateD(t_pipe* this) {
	_pipe_switchToStateB(this);
	this->state = 'D';
}

