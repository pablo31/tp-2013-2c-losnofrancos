#include "nivel.h"

#ifndef NIVEL_CONFIGURACION
#define NIVEL_CONFIGURACION

#include "../libs/common.h"

void cargar_configuracion_cambiante(tad_nivel* nvl, t_config* config,
		char* as_out algoritmo, int as_out quantum, int as_out retardo);
void cargar_recursos_nivel(tad_nivel* nivel);
tad_nivel* crear_nivel(char* config_path, char* as_out ippuerto);
void destruir_nivel(tad_nivel* nivel);
#endif
