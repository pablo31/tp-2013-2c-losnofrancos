#ifndef VERFICADOR_DEADLOCK_H_
#define VERFICADOR_DEADLOCK_H_

#include "nivel.h"

void* verificador_deadlock(void* nivel);
void liberar_recursos_del_personaje(t_list* personajes,	t_list* recursos_disponibles);

#endif
