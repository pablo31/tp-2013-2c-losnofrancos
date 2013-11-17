#ifndef VERIFICADOR_DEADLOCK_H_
#define VERIFICADOR_DEADLOCK_H_

#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/common/collections/list.h"


class (t_personaje_bloqueado){
	char simbolo;
	char* nombre;
};

int encontre_recurso (tad_recurso* recurso1, tad_recurso* recurso2);

int verificador_deadlock(tad_nivel* nivel);

void liberar_recursos_del_personaje(tad_personaje* personaje, t_list* lista_recursos);

#endif
