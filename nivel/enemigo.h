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

#define EJE_X 0
#define EJE_Y 1

void movimiento_permitido_enemigo(PACKED_ARGS);
void moverse_sin_personajes(tad_nivel* nivel, tad_enemigo* self);
void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self, int *eje_prox_mov);
bool posicion_valida(tad_nivel* nivel, vector2 pos);
vector2 esquivar_posicion(vector2 posicion_actual, vector2 nueva_posicion, vector2 posicion_personaje);
int calcular_direccion_movimiento(vector2 pos1, vector2 pos2);
int calcular_distancia(vector2 posicion_a, vector2 posicion_b);
vector2 buscar_personaje_mas_cercano(tad_nivel* nivel, tad_enemigo* self, vector2 posicion_actual);
vector2 movimiento_random(vector2 enemigo_pos, int random, int cantidad);
vector2 moverse_una_posicion(vector2 posicion, int eje, int sentido);
vector2 movimiento_en_L(vector2 enemigo_pos, int eje, int sentido, int random, int cantidad);
int get_eje_alterno(int eje);


#endif


