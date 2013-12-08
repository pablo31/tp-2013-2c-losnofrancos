#include "anim.h"

static void _anim_update(t_anim* anim);
static void _anim_render(t_anim* anim);

t_anim* anim_create(void (*init)(t_anim*), void (*update)(t_anim*), int refreshTime) {
	t_screen* screen = screen_create();
	if (screen == NULL) return NULL;

	t_anim* this = new(t_anim);
	this->screen = screen;
	this->init = init;
	this->update = update;
	this->objects = list_create();
	this->refreshTime = refreshTime;
	this->stop = false;

	return (t_anim*) this;
}

void anim_play(t_anim* anim) {
	anim->init(anim);
	anim->stop = false;
	while(!anim->stop) {
		_anim_update(anim);
		_anim_render(anim);
	}
}

void anim_addToRenderList(t_anim* anim, void* object) {
	list_add(anim->objects, (i_drawable*) object);
}

void anim_removeFromRenderList(t_anim* anim, void* object) {
	bool _remove(void* item) { return item == object; }
	list_remove_by_condition(anim->objects, _remove);
}

void anim_removeAndDestroyFromRenderList(t_anim* anim, void* object) {
	bool _remove(void* item) { return item == object; }
	list_remove_and_destroy_by_condition(anim->objects, _remove, free);
}

void anim_destroy(t_anim* anim) {
	list_destroy_and_destroy_elements(anim->objects, free);

	screen_destroy(anim->screen);
	free(anim);
}

static void _anim_update(t_anim* anim) {
	anim->update(anim);

	void _update(i_drawable* drawable) {
		if (drawable->update != NULL)
			drawable->update(drawable);
	}
	list_iterate(anim->objects, (void*) _update);
}

static void _anim_render(t_anim* anim) {
	screen_preRefresh(anim->screen);

	void _render(i_drawable* drawable) {
		screen_renderItem(anim->screen, drawable);
	}
	list_iterate(anim->objects, (void*) _render);

	usleep(anim->refreshTime * 1000);

	screen_refresh(anim->screen);
}
