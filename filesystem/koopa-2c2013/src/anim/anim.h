#ifndef ANIM_H_
#define ANIM_H_

	#include "headers.h"
	#include "screen.h"

	typedef struct t_anim {
		t_screen* screen;
		void (*init)(struct t_anim*);
		void (*update)(struct t_anim*);
		t_list* objects;
		int refreshTime;
		bool stop;
	} t_anim;

	t_anim* anim_create(void (*init)(t_anim*), void (*update)(t_anim*), int refreshTime);
	void anim_play(t_anim* anim);
	void anim_addToRenderList(t_anim* anim, void* object);
	void anim_removeFromRenderList(t_anim* anim, void* object);
	void anim_removeAndDestroyFromRenderList(t_anim* anim, void* object);
	void anim_destroy(t_anim* anim);

#endif
