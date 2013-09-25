/*
 * plataforma.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/common/collections/list.h"

#include "../libs/logger/logger.h"
#include "../libs/signal/signal.h"
#include "../libs/common.h"

#include "plataforma.h"


int main(int argc, char **argv){
	//char* puerto_orquestador = argv[1];
	char* nombre_ejecutable = argv[0];

	if (argc < 1){
		printf("\tDebe especificar el archivo de configuracion de los planificadores.\n");
		printf("\tej: plataforma.sh planfificador.conf\n");
		return EXIT_FAILURE;
	}

	//char* configuracion = argv[1];
	//TODO

	logger_initialize_for_info("fruta.log", nombre_ejecutable);

	tad_plataforma* plataforma = plataforma_crear();

	logger_info(plataforma_get_logger(plataforma), "Proceso Plataforma iniciado");

	//Establecemos la señal para poder finalizar plataforma
	signal_declare_handler(SIGINT, plataforma_finalizar, 1, plataforma);
	logger_info(plataforma_get_logger(plataforma), "Signals establecidas");

	//Ejecutamos el orquestador en el hilo principal
	orquestador_ejecutar(plataforma_get_orquestador(plataforma));

	//Dado que hay un ciclo infinito en el orquestador, no deberia llegar hasta aca
	return EXIT_FAILURE;
}

void plataforma_finalizar(PACKED_ARGS){
	UNPACK_ARG(tad_plataforma* plataforma);

	//finalizo el orquestador
	orquestador_finalizar(plataforma_get_orquestador(plataforma));
	//finalizo los planificadores
	//TODO finalizar planificadores

	//finalizo el logger de plataforma
	logger_dispose_instance(plataforma_get_logger(plataforma));
	//libero los recursos del singleton logger
	logger_dispose();
	//libero los recursos de las senales
	signal_dispose_all();

	//finalizo el programa
	exit(EXIT_SUCCESS);
}

tad_plataforma* plataforma_crear(){
	//alojamos la estructura tad_plataforma
	obj_alloc(ret, tad_plataforma);
	//creamos el orquestador
	ret->orquestador = orquestador_crear(ret);
	//creamos la lista de planificadores
	ret->planificadores = list_create();
	//creamos una instancia del logger
	ret->logger = logger_new_instance("Plataforma");

	return ret;
}

tad_logger* plataforma_get_logger(tad_plataforma* plataforma){
	return plataforma->logger;
}
tad_orquestador* plataforma_get_orquestador(tad_plataforma* plataforma){
	return plataforma->orquestador;
}

//Nos dice si el planificador de cierto nivel ya se encuentra iniciado
int plataforma_planificador_iniciado(tad_plataforma* plataforma, int nro_nivel){
	foreach(planificador, plataforma->planificadores, tad_planificador*)
		if(planificador_numero_nivel(planificador) == nro_nivel)
			return 1;
	return 0;
}

//Inicia el planificador para un numero de nivel dado
void plataforma_iniciar_planificador(tad_plataforma* plataforma, int nro_nivel){
	//TODO
}
