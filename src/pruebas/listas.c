/*
 * listas.c
 *
 *  Created on: May 5, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../lista/lista_recorrible.h"
#include "../libs/common/collections/list.h"
#include "../libs/common.h"

int main(void){

	printf("Pruebas > Listas recorribles\nInicializando lista...\n");

//	//Escritura
//	int* elemento = malloc(sizeof(int));
//	*elemento = 0;
//	tad_lista_recorrible* lista = lista_recorrible_crear();
//	printf("Lista inicializada\n");
//	lista_recorrible_agregar(lista, elemento);
//	elemento = malloc(sizeof(int));
//	*elemento = 1;
//	lista_recorrible_agregar(lista, elemento);
//	elemento = malloc(sizeof(int));
//	*elemento = 4;
//	lista_recorrible_agregar(lista, elemento);
//	printf("Elementos agregados\n");
//
//	//Lectura
//	elemento = lista_recorrible_elemento_actual(lista);
//	printf("Primer elemento: %d\n", *elemento);
//	lista_recorrible_avanzar(lista);
//	elemento = lista_recorrible_elemento_actual(lista);
//	printf("Segundo elemento: %d\n", *elemento);
//	elemento = lista_recorrible_elemento_actual(lista);
//	printf("Segundo elemento bis: %d\n", *elemento);
//	lista_recorrible_avanzar(lista);
//	elemento = lista_recorrible_elemento_actual(lista);
//	printf("Tercer elemento: %d\n", *elemento);
//
//	lista_recorrible_destruir(lista);

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
