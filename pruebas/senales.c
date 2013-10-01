/*
 * senales.c
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libs/signal/signal.h"
#include "../libs/common.h"

private void sigint_handler(PACKED_ARGS);

int main(void){
	printf("\n > Prueba de senales\n");
	printf(" > \tMuestra como establecer una funcion manejadora para una senal.\n");

	printf("Prueba de senales iniciada.\nEstableciendo senales...\n");

	char* text = "\nSIGINT recibido con exito!\n";
	signal_declare_handler(SIGINT, sigint_handler, 1, text);

	printf("Senales establecidas.\n");
	while(1) sleep(2);

	return EXIT_FAILURE;
}

private void sigint_handler(PACKED_ARGS){
	UNPACK_ARG(char* text);

	printf("%s\n", text);
	signal_dispose_all();
	exit(EXIT_SUCCESS);
}
