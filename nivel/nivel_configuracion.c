#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/common/config.h"
#include "../libs/logger/logger.h"
#include "../libs/common/string.h"
#include "../libs/common/collections/list.h"
#include "nivel_configuracion.h"

void destruir_nivel(nivel* nivel){
	//tp_logger_info(logger, "Liberando recursos del nivel.");
	
	//tp_logger_debug(logger, "Liberando lista de cajas");
	list_destroy(nivel->cajas);
	//free(nvl->nombre);
	//free(nvl->orquestador);
	free(nivel);
}

static caja* crear_caja(char* nombre,char simbolo, uint instancias, uint pos_x, uint pos_y){
	alloc(caja,caja);
	
	caja->nombre = "";
	caja->nombre = strdup(nombre);
	caja->simbolo = simbolo;
	caja->instancias = instancias;
	caja->pos_x = pos_x;
	caja->pos_y = pos_y;
	
	return caja;
}

static void crear_enemigos(nivel* nivel, uint cantidad){
	int i;
	int seed = time(NULL);

	for (i = 0; i < cantidad; ++i){

		enemigo* enemigo = malloc(sizeof(enemigo));
		enemigo->simbolo = '*';

		srand (++seed);//le agrego una a la semilla para que no genere siempre lo mismo en cada iteracion
		enemigo->pos_x = 1 + (rand() % 15);//le sumo 1 porque no puede ser 0 0 nunca.
		enemigo->pos_y = 1 + (rand() % 15);

		list_add(nivel->enemigos, enemigo);	
	}	
}

void cargar_configuracion_cambiante(nivel* nvl, t_config* config,
		char* as_out algoritmo, int as_out quantum, int as_out retardo){
	set algoritmo = string_duplicate(config_get_string_value(config,"algoritmo"));
	set quantum = config_get_int_value(config,"quantum");
	set retardo = config_get_int_value(config,"retardo");
}

static void cargar_configuracion_nivel(nivel* nvl, t_config* config){	
	logger_info(nvl->logger, "Cargando configuracion del nivel");

	nvl->nombre = string_duplicate(config_get_string_value(config, "Nombre"));
	logger_info(nvl->logger, "Nombre:%s", nvl->nombre);

	nvl->tiempo_deadlock = config_get_int_value(config,"TiempoChequeoDeadlock");
	logger_info(nvl->logger, "Deadlock:%s", (nvl->tiempo_deadlock) ? "si" : "no" );

	nvl->recovery = (bool) config_get_int_value(config,"Recovery");
	logger_info(nvl->logger, "Recovery:%i",nvl->tiempo_deadlock);

	uint enemigos = config_get_int_value(config,"Enemigos");
	logger_info(nvl->logger, "Enemigos:%i",enemigos);

	nvl->sleep_enemigos = config_get_int_value(config,"Sleep_Enemigos");
	logger_info(nvl->logger, "Sleep Enemigos:%i", nvl->sleep_enemigos);

	nvl->plataforma = string_duplicate(config_get_string_value(config,"Plataforma"));
	logger_info(nvl->logger, "Plataforma:%s", nvl->plataforma);

	cargar_configuracion_cambiante(nvl, config, out nvl->algoritmo, out nvl->quantum, out nvl->retardo);
	logger_info(nvl->logger, "Algoritmo:%s", nvl->algoritmo);
	logger_info(nvl->logger, "Quantum:%i", nvl->quantum);
	logger_info(nvl->logger, "Retardo:%i", nvl->retardo);
	

	uint  numero_caja = 1;
	char* nombre_caja = string_from_format("Caja%i",numero_caja);
	char* nombre =  "";
	char  simbolo ;
	uint  instancias;
	uint  pos_x;
	uint  pos_y;

	char *p;
	const int base = 10;
	logger_info(nvl->logger ,"Cajas");
	logger_info(nvl->logger ,"");
	while(config_has_property(config, nombre_caja)){

		logger_info(nvl->logger ,"\t%s",nombre_caja);

		char** valores = config_get_array_value(config,nombre_caja );
		
		nombre = valores[0];
		logger_info(nvl->logger ,"\tNombre:%s",nombre);
		simbolo = valores[1][0];
		logger_info(nvl->logger ,"\tSimbolo:%c",simbolo);
		instancias = strtol(valores[2], &p, base);
		logger_info(nvl->logger ,"\tInstancias:%i",instancias);
		pos_x = strtol(valores[3], &p, base);
		logger_info(nvl->logger ,"\tPosicion eje x:%i",pos_x);		
		pos_y = strtol(valores[4], &p, base);
		logger_info(nvl->logger ,"\tPosicion eje y:%i",pos_y);
		
		logger_info(nvl->logger ,"");
		caja* caja = crear_caja(nombre,simbolo,instancias,pos_x,pos_y);		
		list_add(nvl->cajas, caja);	
		
		numero_caja++;
		nombre_caja = string_from_format("Caja%i",numero_caja);
	}

	crear_enemigos(nvl,enemigos);
}

nivel* crear_nivel(char* config_path) {
	alloc(nvl, nivel);
	
	nvl->logger = logger_new_instance("");
	t_config* config = config_create(config_path);

	nvl->nombre = "";
	nvl->plataforma = "";
	nvl->tiempo_deadlock = 0;
	nvl->recovery = false;
	nvl->cajas = malloc(sizeof(caja*));
	nvl->cajas = list_create();
	nvl->enemigos = malloc(sizeof(enemigo*));
	nvl->enemigos = list_create();
	
	cargar_configuracion_nivel(nvl,config);
	config_destroy(config);

	return nvl;
}
