#ifndef VERIFICADOR_DEADLOCK_H_
#define VERIFICADOR_DEADLOCK_H_

#include "nivel.h"
#include "../libs/common/collections/list.h"

typedef struct {
	t_list* personajes_listos;
	t_list* personajes_bloqueados;

} t_verificador_deadlock;


void* verificador_deadlock(tad_nivel* nivel);
t_list* clonar_recursos(tad_nivel* nivel);
t_list* clonar_personajes(tad_nivel* nivel);
void liberar_recursos_del_personaje(t_list* personajes_listos,	t_list* recursos_disponibles);

#endif
