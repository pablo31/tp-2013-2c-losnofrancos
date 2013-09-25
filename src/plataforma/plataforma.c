/*
 * plataforma.c
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/logger/logger.h"
#include "../libs/signal/signal.h"

#include "plataforma.h"

//#define plataforma_log(plataforma, tipo, texto) logger_##tipo(plataforma_get_logger(plataforma), texto)

#define plataforma_log(plataforma, level_name, ...) \
		logger_##level_name##_valt(plataforma_get_logger(plataforma), __VA_ARGS__)

int main(int argc, char **argv){
	//char* puerto_orquestador = argv[1];
	char* nombre_ejecutable = argv[0];

	if (argc < 1){
		printf("\tDebe especificar el archivo de configuracion de los planificadores.\n");
		printf("\tej: plataforma.sh planfificador.conf\n");
		return EXIT_FAILURE;
	}

	//char* configuracion = argv[1];
	//TODO usarla de alguna forma con tp_plataforma_iniciar_planificador

	logger_initialize_for_info("fruta", nombre_ejecutable);

	tad_plataforma* plataforma = plataforma_crear();

	plataforma_log(plataforma, info, "Proceso Plataforma iniciado");

	//Establecemos la señal para poder finalizar plataforma
	signal_declare_handler(SIGINT, plataforma_finalizar, 1, plataforma);
	plataforma_log(plataforma, info, "Signals establecidas");

	//Ejecutamos el orquestador en el hilo principal
	orquestador_ejecutar(plataforma_get_orquestador(plataforma));

	//Dado que hay un ciclo infinito en el orquestador, no deberia llegar hasta aca
	//Pero por si las moscas...
	plataforma_finalizar(plataforma);
	return EXIT_FAILURE;
}
