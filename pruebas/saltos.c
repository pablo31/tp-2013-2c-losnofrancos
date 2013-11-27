/*
 * saltos.c
 *
 *  Created on: Jun 5, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include "../libs/error/jump.h"
#include "../libs/common.h"

typedef enum { false = 0, true = 1} bool;

process_status sps; //stable process status

private void execution(){
	//salvamos el estado del programa
	bool segunda_pasada = save_process_status(sps);

	if(segunda_pasada){
		//hilo alternativo
		printf("Salto completado con exito!\n");
		return;
	}

	//hilo normal del programa
	printf("Saltando en 3...\n");
	printf("Saltando en 2...\n");
	printf("Saltando en 1...\n");

	load_process_status(sps); //cargamos el estado

	printf("Si este mensaje se imprime salio todo mal.\n");
}

int main(){
	printf("\n > Prueba de saltos\n");
	printf(" > \tMuestra como salvar un estado del programa y luego restaurarlo.\n");
	printf(" > \tEsto puede ser utilizado para implementar excepciones o saltos sin retorno.\n");
	printf("Prueba iniciada.\n");
	execution();
	printf("Prueba finalizada.\n\n");
	return EXIT_SUCCESS;
}
