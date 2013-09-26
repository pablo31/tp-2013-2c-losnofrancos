/*
 * protocol.c
 *
 *  Created on: Sep 26, 2013
 *      Author: pablo
 */

#include <stdlib.h>
#include <string.h>



void deserializar_nombre_simbolo_nivel(void* data, char** out_nombre, char* out_simbolo, int* out_nivel){
	int longitud_nombre = strlen((char*) data);
	char* nombre = malloc(longitud_nombre + 1);
	memcpy(nombre, data, longitud_nombre + 1);

	char* simbolo = malloc(sizeof(char));
	*simbolo = *(data + longitud_nombre + 1);

	int* nivel = malloc(sizeof(int));
	*nivel = *(data + longitud_nombre + 2);

	*out_nombre = &nombre;
	*out_simbolo = &simbolo;
	*out_nivel = &nivel;
}
