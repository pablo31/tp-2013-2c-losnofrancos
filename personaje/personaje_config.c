/*
 * personaje_config.c
 *
 *  Created on: Nov 30, 2013
 *      Author: pablo
 */


#include "../libs/common/string.h"
#include "../libs/common/config.h"


#include "personaje.h"


t_personaje* personaje_crear(char* config_path){
	//creamos una instancia de personaje
	alloc(self, t_personaje);

	//creamos una instancia del lector de archivos de config
	t_config* config = config_create(config_path);

	self->nombre = string_duplicate(config_get_string_value(config, "nombre"));
	self->simbolo = *config_get_string_value(config, "simbolo");

	//cargamos los datos del logger
	char* log_file = config_get_string_value(config, "logFile");
	char* log_level = config_get_string_value(config, "logLevel");
	logger_initialize(log_file, "personaje.sh", log_level);

	//obtenemos una instancia del logger
	self->logger = logger_new_instance();

	int vidas = config_get_int_value(config, "vidas");
	self->vidas_iniciales = vidas;
	self->vidas = vidas;

	self->ippuerto_orquestador = string_duplicate(config_get_string_value(config, "orquestador"));


	int cantidad_niveles = config_get_int_value(config, "niveles");
	char** nombres_niveles = config_get_array_value(config, "planDeNiveles");

	t_list* niveles = list_create();
	int i;
	for(i = 0; i < cantidad_niveles; i++){
		alloc(nivel, t_nivel);
		char* nombre_nivel = nombres_niveles[i];
		nivel->nombre = nombre_nivel;
		nivel->objetivos = list_create();

		char* key_cantidad_objetivos = string_from_format("objs[%s]", nombre_nivel);
		char* key_objetivos = string_from_format("obj[%s]", nombre_nivel);

		int cantidad_objetivos = config_get_int_value(config, key_cantidad_objetivos);
		char** objetivos = config_get_array_value(config, key_objetivos);

		int ii;
		for(ii = 0; ii < cantidad_objetivos; ii++){
			char* objetivo = objetivos[ii];
			list_add(nivel->objetivos, objetivo);
		}

		free(key_cantidad_objetivos);
		free(key_objetivos);

		list_add(niveles, nivel);
	}
	self->niveles = niveles;

	//liberamos recursos
	config_destroy(config);

	return self;
}
