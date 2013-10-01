/*
 * argumentos.c
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#include <stdlib.h>

#include "../common/collections/list.h"
#include "../libs/common.h"

#include "arguments.h"


//Crea una nueva lista de argumentos
tad_arguments* arguments_new(){
	alloc(ret, tad_arguments);
	ret->list = list_create();
	ret->last_index = 0;
	ret->self_destroy = 0;
	return ret;
}

//Establece que el objeto dure un solo uso
void arguments_self_destroy(tad_arguments* arguments){
	arguments->self_destroy = 1;
}

//Agrega un argumento a la lista
void arguments_add(tad_arguments* arguments, void* new_argument){
	list_add(arguments->list, new_argument);
}

//Obtiene el siguiente argumento y libera sus recursos
void* arguments_get(tad_arguments* arguments){
	void* ret;
	t_list* arguments_list = arguments->list;

	if(arguments->self_destroy)
	{
		ret = list_get(arguments_list, 0);
		list_remove(arguments_list, 0);

		if(list_size(arguments_list) == 0) arguments_dispose(arguments);
	}else{
		int i = arguments->last_index;
		ret = list_get(arguments_list, i);

		i++;
		if(i >= list_size(arguments_list)) i = 0;
		arguments->last_index = i;
	}

	return ret;
}

//Libera recursos
void arguments_dispose(tad_arguments* arguments){
	list_destroy(arguments->list);
	dealloc(arguments);
}
