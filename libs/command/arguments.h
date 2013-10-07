/*
 * argumentos.h
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include "../common/collections/list.h"


struct s_arguments{
	t_list* list;
	int last_index;
	int self_destroy;
};
typedef struct s_arguments tad_arguments;


//Crea un nuevo paquete de argumentos
tad_arguments* arguments_new();

//Setea al paquete de argumentos como auto destructiva o liberadora de recursos en el primer uso
void arguments_self_destroy(tad_arguments* arguments);

//Agrega un argumento al paquete de argumentos
void arguments_add(tad_arguments* arguments, void* new_argument);
//Obtiene el siguiente argumento de un paquete de argumentos
void* arguments_get(tad_arguments* arguments);

//Libera los recursos del paquete de argumentos (si no es auto destructivo)
void arguments_dispose(tad_arguments* arguments);


#endif /* ARGUMENTS_H_ */
