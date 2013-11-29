
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

#define ARRIBA 0
#define ABAJO 1
#define IZQUIERDA 2
#define DERECHA 3

void movimiento_permitido_enemigo(PACKED_ARGS);
void mover_en_L(tad_nivel* nivel,tad_enemigo* self);
void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self);
bool posicion_sin_caja(tad_nivel* nivel, vector2 nueva_pos);
vector2 esquivar_caja(vector2 posicion_actual,vector2 nueva_posicion, tad_nivel* nivel);
int calcular_direccion_movimiento(vector2 pos1, vector2 pos2);
int calcular_distancia (vector2 posicion_a, vector2 posicion_b);
vector2 buscar_personaje_mas_cercano(tad_nivel* nivel, tad_enemigo* self);

#endif


