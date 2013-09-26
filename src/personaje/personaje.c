/*
 * personaje.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <libs/common/string.h> // verificar este paht!!!!
#include "personaje.h"

bool verificar_argumentos(int argc, char* argv[]);
sig_atomic_t reiniciar_nivel = 0;
sig_atomic_t reiniciar_flan = 0;

int main(int argc, char* argv[]) {

	if (!verificar_argumentos(argc, argv)) {
		return EXIT_FAILURE;
	}


	//personaje create levanta archivo de configuracion
	//t_personaje* self = personaje_create(argv[1]);

	if (self == NULL ) {
		return EXIT_FAILURE;
	}

	//se verifican las señales
	void sigterm_handler(int signum) {
		morir(self, "Muerte por señal");
	}

	//estoy hablando de faso XD XD XD
	void sigusr1_handler(int signum) {
			comer_honguito_verde(self);
	}
	//------------fin de señales

	struct sigaction sigterm_action;

	sigterm_action.sa_handler = sigterm_handler;
	sigemptyset(&sigterm_action.sa_mask);
	if (sigaction(SIGTERM, &sigterm_action, NULL ) == -1) {
		log_error(self->logger, "Error al querer setear la señal SIGTERM");
		return EXIT_FAILURE;
	}

	struct sigaction sigusr1_action;

	sigusr1_action.sa_handler = sigusr1_handler;
	sigusr1_action.sa_flags = SA_RESTART;
	sigemptyset(&sigusr1_action.sa_mask);
	if (sigaction(SIGUSR1, &sigusr1_action, NULL ) == -1) {
		log_error(self->logger, "Error al querer setear la señal SIGUSR1");
		return EXIT_FAILURE;
	}



	if (!personaje_conectar_a_nivel(self)) {
		personaje_destroy(self);
		return EXIT_FAILURE;
	}

	if (!personaje_conectar_a_planificador(self)) {
		personaje_destroy(self);
		return EXIT_FAILURE;
	}

}


void morir(t_personaje* self, char* motivo) {
	log_info(self->logger, "Personaje: %s", motivo);
	if (self->vidas > 0) {
		self->vidas--;
		reiniciar_nivel = 1;
	} else {
		self->vidas = self->vidas_iniciales;
		reiniciar_flan = 1;
		log_info(self->logger, "Personaje %s: Reiniciando vidas", self->nombre);
	}
	log_info(self->logger, "Personaje %s: Ahora me quedan %d vidas",
			self->nombre, self->vidas);
}



void comer_honguito_verde(t_personaje* self) {
	log_info(self->logger,
			"Personaje %s: Llegó un honguito de esos que pegan ;)",
			self->nombre);
	self->vidas++;
	log_info(self->logger, "Personaje %s: Ahora tengo %d vidas", self->nombre,
			self->vidas);
}

bool verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error en la cantidad de argumentos.\n");
		return false;
	}
	return true;
}

