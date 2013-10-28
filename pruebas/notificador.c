/*
 * notificador.c
 *
 *  Created on: Oct 28, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libs/signal/signal.h"
#include "../libs/notifier/notifier.h"

void sigint_handler(tad_notifier* notifier){
	notifier_dispose(notifier);
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	if (argc < 2){
		printf("Error: debe ingresar el nombre del archivo a monitorear.\n");
		exit(EXIT_FAILURE);
	}

	printf("\n > Prueba del notificador de cambios\n");
	printf(" > \tMuestra como trabajar con el notificador para monitorear cambios en un archivo.\n");

	char* file_path = argv[1];

	tad_notifier* notifier = notifier_create(file_path);

	signal_dynamic_handler(SIGINT, sigint_handler(notifier));

	while(1){
		notifier_wait_for_modification(notifier);
		printf("El archivo fue modificado.\n");
	}

	return EXIT_FAILURE;
}
