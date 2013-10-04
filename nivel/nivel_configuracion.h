#include "nivel.h"

#ifndef NIVEL_CONFIGURACION
#define NIVEL_CONFIGURACION

bool cargar_configuracion_nivel(nivel* nivel);
void cargar_recursos_nivel(nivel* nivel);
nivel* crear_nivel();
void destruir_nivel(nivel* nivel);
#endif