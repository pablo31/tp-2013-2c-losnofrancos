#ifndef POSICION_H_
#define POSICION_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define DISTANCIA_MOVIMIENTO_PERMITIDA 1

typedef struct t_posicion {
	uint32_t x;
	uint32_t y;
}__attribute__ ((packed)) t_posicion;


//funciones que uso para mover un personaje
//el personaje sabe si esta parado en la posicion o no...
//esto lo uso para llegar al recurso...

t_posicion* posicion_create(int x, int y);
void posicion_destroy(t_posicion* self);
t_posicion* posicion_duplicate(t_posicion* posicion);
bool posicion_equals(t_posicion* self, t_posicion* other);
t_posicion* posicion_get_proxima_hacia(t_posicion* self,
		t_posicion* posicion_destino);
int posicion_get_distancia(t_posicion* self, t_posicion* otra_posicion);

#endif /* POSICION_H_ */
