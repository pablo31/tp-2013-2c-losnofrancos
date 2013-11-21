
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/thread/thread.h"
#include "../libs/vector/vector2.h"

void movimiento_permitido_enemigo(PACKED_ARGS);
void mover_en_L(tad_nivel* nivel,tad_enemigo* self);
void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self);

#endif


