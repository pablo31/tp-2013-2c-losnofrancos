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
	t_personaje* self = personaje_create(argv[1]);

	if (self == NULL ) {
		return EXIT_FAILURE;
	}


	log_debug(self->logger, "Personaje %s creado", self->nombre);


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



	// esto no llegue...  =(
	if (!personaje_conectar_a_nivel(self)) {
		personaje_destroy(self);
		return EXIT_FAILURE;
	}

	if (!personaje_conectar_a_planificador(self)) {
		personaje_destroy(self);
		return EXIT_FAILURE;
	}


	personaje_destroy(self);

	return EXIT_SUCCESS;
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

t_personaje* personaje_create(char* config_path) {
	t_personaje* new = malloc(sizeof(t_personaje));
	t_config* config = config_create(config_path); //commons... andres para nivel es igual...

	new->nombre = string_duplicate(config_get_string_value(config, "nombre"));

	char* s = string_duplicate(config_get_string_value(config, "simbolo"));
	new->simbolo = s[0];

	new->plan_de_niveles = config_get_array_value(config, "planDeNiveles");
	new->objetivos = _personaje_load_objetivos(config, new->plan_de_niveles);
	new->vidas = config_get_int_value(config, "vidas");
	new->orquestador_info = connection_create(
			config_get_string_value(config, "orquestador"));

	void morir(char* mensaje) {
		config_destroy(config);
		free(s);
		personaje_destroy(new);
		printf("Error en el archivo de configuración: %s\n", mensaje);
	}

	if (!config_has_property(config, "puerto")) {
		morir("Falta el puerto");
		return NULL ;
	}
	new->puerto = config_get_int_value(config, "puerto");

	char* log_file = "personaje.log";
	char* log_level = "INFO";
	if (config_has_property(config, "logFile")) {
		log_file = string_duplicate(config_get_string_value(config, "logFile"));
	}
	if (config_has_property(config, "logLevel")) {
		log_level = string_duplicate(
				config_get_string_value(config, "logLevel"));
	}
	new->logger = log_create(log_file, "Personaje", true,
			log_level_from_string(log_level));
	config_destroy(config);

	new->socket_orquestador = NULL;
	new->nivel_actual = NULL;
	new->posicion = NULL;
	new->posicion_objetivo = NULL;
	new->nivel_finalizado = false;
	new->nivel_actual_index = 0;
	new->vidas_iniciales = new->vidas;
	new->objetivos_array = NULL;
	new->objetivo_actual = NULL;
	new->objetivo_actual_index = 0;
	new->is_blocked = false;

	free(s);
	free(log_file);
	free(log_level);
	return new;
}



void personaje_destroy(t_personaje* self) {
	free(self->nombre);
	array_destroy(self->plan_de_niveles);
	dictionary_destroy_and_destroy_elements(self->objetivos,
			(void*) array_destroy);
	connection_destroy(self->orquestador_info);
	log_destroy(self->logger);
	if (self->socket_orquestador != NULL ) {
		if (self->socket_orquestador->serv_socket != NULL ) {
			sockets_destroy(self->socket_orquestador->serv_socket);
		}
		sockets_destroyClient(self->socket_orquestador);
	}
	if (self->nivel_actual != NULL ) {
		personaje_nivel_destroy(self->nivel_actual);
	}
	if (self->posicion != NULL ) {
		posicion_destroy(self->posicion);
	}

	if (self->posicion_objetivo != NULL ) {
		posicion_destroy(self->posicion_objetivo);
	}

	if (self->objetivo_actual != NULL ) {
		free(self->objetivo_actual);
	}

	free(self);
}
