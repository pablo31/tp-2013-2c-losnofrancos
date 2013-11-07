#ifndef VERIFICADOR_DEADLOCK_H_
#define VERIFICADOR_DEADLOCK_H_

#include "nivel.h"
#include "../libs/common/collections/list.h"

void* verificador_deadlock(void* nivel);
t_list* clonar_recursos(tad_nivel* nivel);
t_list* clonar_personajes(tad_nivel* nivel);
void liberar_recursos_del_personaje(t_list* personajes,	t_list* recursos_disponibles);

#endif
