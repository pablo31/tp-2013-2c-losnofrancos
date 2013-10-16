/*
 * listas.c
 *
 *  Created on: May 5, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/collection/round.h"
#include "../libs/common/collections/list.h"
#include "../libs/common.h"

int main(void){

	printf("Pruebas > Listas recorribles\n");

	//Prueba de lista circular

	t_round* round = round_create();

	alloc(elem, int);
	*elem = 1;
	round_add(round, elem);
	ralloc(elem);
	*elem = 2;
	round_add(round, elem);
	ralloc(elem);
	*elem = 3;
	round_add(round, elem);

	while(!round_has_ended(round)){
		elem = round_get(round);
		printf("%d\n", *elem);
		round_foward(round);
	}

	round_restart(round);

	while(!round_has_ended(round)){
		elem = round_remove(round);
		dealloc(elem);
	}

	round_dispose(round);


	//Prueba del macro foreach

	t_list* lista = list_create();
	char* a = "elemento 1!";
	char* b = "elemento 2!";
	char* c = "elemento 3!";

	list_add(lista, a);
	list_add(lista, b);
	list_add(lista, c);

	foreach(item, lista, char*){
		printf("%s\n", item);
	}

	list_destroy(lista);

	printf("Prueba finalizada.\n");

	return EXIT_SUCCESS;
}
