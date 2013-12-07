/*
 * personaje_config.c
 *
 *  Created on: Nov 30, 2013
 *      Author: pablo
 */


#include "../libs/common/string.h"
#include "../libs/common/config.h"


#include "personaje.h"

static void liberar_niveles(char** niveles){
	if (NULL == niveles)
		return;

	uint i = 0;
	while(niveles[i] != NULL){
		free(niveles[i]);
		i++;
	}

	free(niveles);
}

t_personaje* personaje_crear(char* config_path){
	//creamos una instancia de personaje
	alloc(self, t_personaje);

	//creamos una instancia del lector de archivos de config
	t_config* config = config_create(config_path);

	self->nombre = string_duplicate(config_get_string_value(config, "Nombre"));
	self->simbolo = *config_get_string_value(config, "Simbolo");

	//cargamos los datos del logger
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

	logger_initialize(log_file, "personaje", log_level, 1);


	//obtenemos una instancia del logger
	self->logger = logger_new_instance();


	int vidas = config_get_int_value(config, "Vidas");
	self->vidas_iniciales = vidas;
	self->vidas = vidas;


	//direccion de plataforma
	char* plataforma;
	t_config* global_config = config_create("global.cfg");
	if(config_has_property(global_config, "Plataforma"))
		plataforma = string_duplicate(config_get_string_value(global_config, "Plataforma"));
	else
		plataforma = string_duplicate(config_get_string_value(config, "Plataforma"));
	config_destroy(global_config);
	self->ippuerto_orquestador = plataforma;


	int cantidad_niveles = config_get_int_value(config, "Niveles");
	char** nombres_niveles = config_get_array_value(config, "PlanDeNiveles");

	t_list* niveles = list_create();
	int i;
	for(i = 0; i < cantidad_niveles; i++){
		alloc(nivel, t_nivel);
		char* nombre_nivel = string_duplicate(nombres_niveles[i]);
		nivel->nombre = nombre_nivel;
		nivel->objetivos = list_create();

		char* key_objetivos = string_from_format("Obj[%s]", nombre_nivel);
		char* objetivos = config_get_string_value(config, key_objetivos);

		logger_info(self->logger, "Nivel: %s Objetivos:%s", nivel->nombre, objetivos);
		int ii = 0;
		while(objetivos != NULL && '\0' != objetivos[ii]){
			char* objetivo = string_from_format("%c" , objetivos[ii]);
			list_add(nivel->objetivos, objetivo);

			logger_info(self->logger, "Objetivo:%s", objetivo);
			ii++;
		}

		free(key_objetivos);

		list_add(niveles, nivel);
	}

	self->niveles = niveles;

	logger_info(self->logger, "Log File:%s", log_file);
	logger_info(self->logger, "Log Level:%s", log_level);
	free(log_file);

	//liberamos recursos
	config_destroy(config);
	liberar_niveles(nombres_niveles);

	return self;
}
