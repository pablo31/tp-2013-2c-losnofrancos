
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/thread/thread.h"
#include "../libs/vector/vector2.h"
#include "../libs/thread/mutex.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/protocol/protocol.h"
#include "../libs/common/collections/list.h"

void movimiento_permitido_enemigo(PACKED_ARGS);
void mover_en_L(tad_nivel* nivel,tad_enemigo* self);
void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self);
bool posicion_sin_caja(tad_nivel* nivel, vector2 nueva_pos);

#endif


