#ifndef VERIFICADOR_DEADLOCK_H_
#define VERIFICADOR_DEADLOCK_H_

#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/common/collections/list.h"
#include "../libs/logger/logger.h"
#include "../libs/thread/thread.h"


class (t_personaje_bloqueado){
	char simbolo;
	char* nombre;
};


void verificador_deadlock(PACKED_ARGS);
void liberar_recursos_del_personaje(tad_personaje* personaje, t_list* recursos_disponibles);
void resolver_deadlock(tad_nivel* nivel, t_list* personajes_deadlock);
t_list* cargar_lista_personajes(tad_nivel* nivel);
t_list* cargar_lista_recursos(tad_nivel* nivel);

#endif

