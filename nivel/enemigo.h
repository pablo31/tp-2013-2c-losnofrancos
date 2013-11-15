
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "nivel.h"
#include "nivel_ui.h"

void movimiento_permitido_enemigo(tad_nivel* nivel, tad_enemigo* self);
void mover_en_L(tad_enemigo* self);
void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self);

#endif
