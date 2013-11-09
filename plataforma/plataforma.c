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
#include "../libs/common/config.h"
#include "../libs/common/string.h"

#include "../libs/logger/logger.h"
#include "../libs/signal/signal.h"
#include "../libs/thread/thread.h"
#include "../libs/common.h"

#include "plataforma.h"





private tad_logger* get_logger(tad_plataforma* self){
	return self->logger;
}

private void verificar_argumentos(int argc, char* argv[]) {
	if(argc > 1) return;
	printf("Error: Debe ingresar el nombre del archivo de configuracion\n");
	exit(EXIT_FAILURE);
}



int main(int argc, char **argv){

	verificar_argumentos(argc, argv);
	char* config_file = argv[1];

	tad_plataforma* self = plataforma_crear(config_file);

	logger_info(get_logger(self), "Proceso Plataforma iniciado");

	//Establecemos la señal para poder finalizar plataforma
	signal_dynamic_handler(SIGINT, plataforma_finalizar(self));
	logger_info(get_logger(self), "Signals establecidas");

	//Ejecutamos el orquestador en el hilo principal
	orquestador_ejecutar(self->orquestador);

	//Dado que hay un ciclo infinito en el orquestador, no deberia llegar hasta aca
	return EXIT_FAILURE;
}

tad_plataforma* plataforma_crear(char* config_file){
	//alojamos la estructura tad_plataforma
	alloc(ret, tad_plataforma);

	t_config* config = config_create(config_file);
	char* puerto_orquestador = string_duplicate(config_get_string_value(config, "puerto"));

	//cargamos los datos del logger
	char* log_file = config_get_string_value(config, "logFile");
	char* log_level = config_get_string_value(config, "logLevel");
	logger_initialize(log_file, "plataforma.sh", log_level);

	config_destroy(config);

	//creamos el orquestador
	ret->orquestador = orquestador_crear(ret, puerto_orquestador);
	//creamos la lista de planificadores
	ret->planificadores = list_create();
	//creamos una instancia del logger
	ret->logger = logger_new_instance("Plataforma");

	return ret;
}

void plataforma_finalizar(tad_plataforma* self){
	//liberamos los recursos de los planificadores
	void liberar_planificador(void* planificador){
		planificador_finalizar(planificador);
	}
	list_destroy_and_destroy_elements(self->planificadores, liberar_planificador);
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
	tad_planificador* planificador = planificador_crear(nombre_nivel, socket_nivel, self);
	//lo agregamos a la lista de planificadores
	list_add(self->planificadores, planificador);
	//ejecutamos el planificador en un nuevo thread
	thread_free_begin(planificador_ejecutar, 1, planificador);
}

//Quita de la lista de planificadores al planificador dado
void plataforma_finalizar_planificador(tad_plataforma* self, tad_planificador* planificador){
	list_remove_where(self->planificadores, tad_planificador* elem, elem == planificador);
	planificador_finalizar(planificador);
}
