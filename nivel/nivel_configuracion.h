#include "nivel.h"

#ifndef NIVEL_CONFIGURACION
#define NIVEL_CONFIGURACION

#include "../libs/common.h"

void cargar_configuracion_cambiante(nivel* nvl, t_config* config,
		char* as_out algoritmo, int as_out quantum, int as_out retardo);
void cargar_recursos_nivel(nivel* nivel);
nivel* crear_nivel();
void destruir_nivel(nivel* nivel);
#endif
