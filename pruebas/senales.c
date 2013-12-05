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

private void sigint_handler();
private void sigterm_handler();
private void sigusr1_handler();

int main(void){
	printf("\n > Prueba de senales\n");
	printf(" > \tMuestra como establecer una funcion manejadora para una senal.\n");

	printf("Prueba de senales iniciada.\nEstableciendo senales...\n");

	char* sigint_text = "SIGINT recibido con exito!\n";
	signal_dynamic_handler(SIGINT, sigint_handler(sigint_text));

	signal_dynamic_handler(SIGTERM, sigterm_handler());
	signal_dynamic_handler(SIGUSR1, sigusr1_handler());

	printf("Senales establecidas: SIGUSR1, SIGTERM, SIGINT.\n");
	while(1) sleep(2);

	return EXIT_FAILURE;
}

private void sigint_handler(char* text){
	printf("%s\n", text);
	signal_dispose_all();
	exit(EXIT_SUCCESS);
}

private void sigterm_handler(){
	printf("SIGTERM recibido con exito!\n");
}

private void sigusr1_handler(){
	printf("SIGUSR1 recibido con exito!\n");
}
