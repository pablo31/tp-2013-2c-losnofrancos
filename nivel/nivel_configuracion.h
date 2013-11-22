#include "nivel.h"

#ifndef NIVEL_CONFIGURACION
#define NIVEL_CONFIGURACION

#include "../libs/common.h"

void cargar_configuracion_nivel(tad_nivel* self, char* as_out ippuerto);
void recargar_configuracion_nivel(tad_nivel* nvl, char* config_file,
		char* as_out algoritmo, int as_out quantum, int as_out retardo);
void cargar_recursos_nivel(tad_nivel* nivel);
void destruir_nivel(tad_nivel* nivel);
#endif
