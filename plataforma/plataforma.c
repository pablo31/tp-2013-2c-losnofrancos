/*
 * plataforma.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/common/collections/list.h"

#include "../libs/logger/logger.h"
#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/common.h"

#include "plataforma.h"





private tad_logger* get_logger(tad_plataforma* self){
	return self->logger;
}
private tad_orquestador* get_orquestador(tad_plataforma* self){
	return self->orquestador;
}





int main(int argc, char **argv){
	//char* puerto_orquestador = argv[1];
	char* nombre_ejecutable = argv[0];

	if (argc < 1){
		printf("\tDebe especificar el archivo de configuracion de los planificadores.\n");
		printf("\tej: plataforma.sh planfificador.conf\n");
		return EXIT_FAILURE;
	}

	//char* configuracion = argv[1];
	//TODO levantar archivo de config

	logger_initialize_for_info("plataforma.log", nombre_ejecutable);

	tad_plataforma* self = plataforma_crear();

	logger_info(get_logger(self), "Proceso Plataforma iniciado");

	//Establecemos la señal para poder finalizar plataforma
	signal_dynamic_handler(SIGINT, plataforma_finalizar(self));
	logger_info(get_logger(self), "Signals establecidas");

	//Ejecutamos el orquestador en el hilo principal
	orquestador_ejecutar(get_orquestador(self));

	//Dado que hay un ciclo infinito en el orquestador, no deberia llegar hasta aca
	return EXIT_FAILURE;
}

tad_plataforma* plataforma_crear(){
	//alojamos la estructura tad_plataforma
	alloc(ret, tad_plataforma);
	//creamos el orquestador
	ret->orquestador = orquestador_crear(ret);
	//creamos la lista de planificadores
	ret->planificadores = list_create();
	//creamos una instancia del logger
	ret->logger = logger_new_instance("Plataforma");

	return ret;
}

void plataforma_finalizar(tad_plataforma* self){
	//liberamos los recursos de los planificadores
	foreach(planificador, self->planificadores, tad_planificador*)
		planificador_finalizar(planificador);
	//liberamos los recursos del orquestador
	orquestador_finalizar(self->orquestador);
	//liberamos los recursos propios de plataforma
	logger_info(get_logger(self), "Finalizando");
	logger_dispose_instance(self->logger);
	dealloc(self);
	//libero los recursos del singleton logger
	logger_dispose();
	//libero los recursos de las senales
	signal_dispose_all();

	exit(EXIT_SUCCESS);
}

//Nos dice si el planificador de cierto nivel ya se encuentra iniciado
tad_planificador* plataforma_planificador_iniciado(tad_plataforma* self, char* nombre_nivel){
	foreach(planificador, self->planificadores, tad_planificador*)
		if(string_equals(planificador_nombre_nivel(planificador), nombre_nivel))
			return planificador;
	return null;
}

//Inicia el planificador para un numero de nivel dado
void plataforma_iniciar_planificador(tad_plataforma* self, char* nombre_nivel, tad_socket* socket_nivel){
	//creamos el planificador
	tad_planificador* planificador = planificador_crear(nombre_nivel, socket_nivel);
	//lo agregamos a la lista de planificadores
	list_add(self->planificadores, planificador);
	//ejecutamos el planificador en un nuevo thread
	thread_free_begin(planificador_ejecutar, 1, planificador);
}
