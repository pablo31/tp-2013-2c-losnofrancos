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


//Creation
tad_arguments* arguments_new();

//Setters
void arguments_self_destroy(tad_arguments* arguments);

//Operation
void arguments_add(tad_arguments* arguments, void* new_argument);
void* arguments_get(tad_arguments* arguments);

//Disposal
void arguments_dispose(tad_arguments* arguments);


#endif /* ARGUMENTS_H_ */
