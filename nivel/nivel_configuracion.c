
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../libs/common/collections/list.h"
#include "../libs/common/config.h"
#include "../libs/common/string.h"

#include "../libs/logger/logger.h"

#include "nivel.h"
#include "nivel_ui.h"
#include "nivel_configuracion.h"

static tad_caja* crear_caja(char* nombre,char simbolo, int instancias, int pos_x, int pos_y){
	alloc(ret, tad_caja);
	
	ret->nombre = "";
	ret->nombre = strdup(nombre);
	ret->simbolo = simbolo;
	ret->instancias = instancias;
	ret->pos = vector2_new(pos_x, pos_y);
	
	return ret;
}

static void crear_enemigos(tad_nivel* nivel, int cantidad){
	int i;
	int seed = time(null);

	for (i = 0; i < cantidad; ++i){

		alloc(enem, tad_enemigo);
		enem->simbolo = '*';

		srand (++seed); //le agrego una a la semilla para que no genere siempre lo mismo en cada iteracion

		int pos_x = 1 + (rand() % 15); //le sumo 1 porque no puede ser 0 0 nunca.
		int pos_y = 1 + (rand() % 15);

		enem->pos = vector2_new(pos_x, pos_y);

		list_add(nivel->enemigos, enem);
	}	
}



private void cargar_configuracion_cambiante(tad_nivel* nvl, t_config* config,
		char* as_out algoritmo, int as_out quantum, int as_out retardo){
	set retardo = config_get_int_value(config,"Retardo");
	set algoritmo = string_duplicate(config_get_string_value(config,"Algoritmo"));

	if (string_equals_ignore_case(*algoritmo, "RR"))
		set quantum = config_get_int_value(config,"Quantum");
	else
		set quantum = 0;
}

void recargar_configuracion_nivel(tad_nivel* nvl, char* config_file,
		char* as_out algoritmo, int as_out quantum, int as_out retardo){

	t_config* config = config_create(config_file);
	cargar_configuracion_cambiante(nvl, config, algoritmo, quantum, retardo);
	config_destroy(config);
}


void cargar_configuracion_nivel(tad_nivel* self, char* as_out ippuerto){
	var(config_path, get_config_path(self));
	var(config, config_create(config_path));

	self->nombre = string_duplicate(config_get_string_value(config, "Nombre"));
	char* log_file;
	if(config_has_property(config,"LogFile"))
		log_file = config_get_string_value(config, "LogFile");
	else 
		log_file = string_from_format("%s.log", self->nombre);

	char* log_level; 
	if(config_has_property(config,"LogLevel"))
	 	log_level = config_get_string_value(config, "LogLevel");		
	else 
		log_level = "INFO";		

	logger_initialize(log_file, "nivel.sh", log_level, 1); //no logea en consola
	var(logger, logger_new_instance());
	self->logger = logger;

	logger_info(logger, "Cargando configuracion del nivel");


	
	logger_info(logger, "Nombre:%s", self->nombre);

	self->tiempo_deadlock = config_get_int_value(config,"TiempoChequeoDeadlock");
	logger_info(logger, "Deadlock:%s", (self->tiempo_deadlock) ? "si" : "no" );

	self->recovery = (bool) config_get_int_value(config,"Recovery");
	logger_info(logger, "Recovery:%i", self->tiempo_deadlock);

	int enemigos = config_get_int_value(config,"Enemigos");
	logger_info(logger, "Enemigos:%i",enemigos);

	self->sleep_enemigos = config_get_int_value(config,"Sleep_Enemigos");
	logger_info(logger, "Sleep Enemigos:%i", self->sleep_enemigos);

	char* plataforma = string_duplicate(config_get_string_value(config,"Plataforma"));
	logger_info(logger, "Plataforma:%s", plataforma);
	set ippuerto = plataforma;

	cargar_configuracion_cambiante(self, config, out self->algoritmo, out self->quantum, out self->retardo);
	logger_info(logger, "Algoritmo:%s", self->algoritmo);
	logger_info(logger, "Quantum:%i", self->quantum);
	logger_info(logger, "Retardo:%i", self->retardo);
	logger_info(logger, "Log File:%s", log_file);
	logger_info(logger, "Log Level:%s", log_level);
	

	uint  numero_caja = 1;
	char* nombre_caja = string_from_format("Caja%i", numero_caja);

	char *p;
	const int base = 10;

	logger_info(logger ,"Cajas");
	logger_info(logger ,"");
	while(config_has_property(config, nombre_caja)){

		logger_info(logger, "\t%s", nombre_caja);

		char** valores = config_get_array_value(config, nombre_caja);
		
		char* nombre = valores[0];
		logger_info(logger ,"\tNombre:%s",nombre);
		char simbolo = valores[1][0];
		logger_info(logger ,"\tSimbolo:%c",simbolo);
		int instancias = strtol(valores[2], &p, base); //TODO no se puede hacer algo mas lindo??
		logger_info(logger ,"\tInstancias:%i",instancias);
		int pos_x = strtol(valores[3], &p, base);
		int pos_y = strtol(valores[4], &p, base);
		logger_info(logger ,"\tPosicion eje x:%i",pos_x);
		logger_info(logger ,"\tPosicion eje y:%i",pos_y);
		
		logger_info(logger ,"");
		tad_caja* caja = crear_caja(nombre,simbolo,instancias,pos_x,pos_y);		
		list_add(self->cajas, caja);
		
		numero_caja++;

		free(nombre_caja);
		nombre_caja = string_from_format("Caja%i", numero_caja);
	}

	crear_enemigos(self,enemigos);
	
	config_destroy(config);
}



void destruir_nivel(tad_nivel* self){
	//liberamos strings varios
	free(self->nombre);
	free(self->algoritmo);

	//liberamos listas
	void liberar_personaje(void* ptr_pj){
		tad_personaje* pj = ptr_pj;
		dealloc(pj);
	}
	list_destroy_and_destroy_elements(self->personajes, liberar_personaje);
	list_destroy(self->cajas); //TODO destruir elementos
	list_destroy(self->enemigos); //TODO destruir elementos

	//liberamos el socket (si esta abierto)
	var(socket, self->socket);
	if(socket != null) socket_close(socket);

	//liberamos el logger
	logger_dispose_instance(self->logger);

	//liberamos el espacio de memoria propio del nivel
	dealloc(self);
}
