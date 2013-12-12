/*
 * personaje_config.c
 *
 *  Created on: Nov 30, 2013
 *      Author: pablo
 */


#include "../libs/common/string.h"
#include "../libs/common/config.h"
#include "../libs/common.h"


#include "personaje.h"

static void liberar_niveles(char** niveles){
	if (null == niveles)
		return;

	uint i = 0;
	while(niveles[i] != null){
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
	t_config* global_config = config_create("global.cfg");

	//datos basicos del personaje
	self->nombre = string_duplicate(config_get_string_value(config, "Nombre"));
	self->simbolo = *config_get_string_value(config, "Simbolo");
	int vidas = config_get_int_value(config, "Vidas");
	self->vidas_iniciales = vidas;
	self->vidas = vidas;

	//datos del logger
	char* log_file;
	if(config_has_property(config,"LogFile")) log_file = string_duplicate(config_get_string_value(config, "LogFile"));
	else log_file = string_from_format("%s.log", self->nombre);

	char* log_level; 
	if(config_has_property(global_config,"LogLevel")) log_level = config_get_string_value(global_config, "LogLevel");
	else if(config_has_property(config,"LogLevel")) log_level = config_get_string_value(config, "LogLevel");
	else log_level = "INFO";

	//inicializamos el logger
	logger_initialize(log_file, "personaje", log_level, 1);
	free(log_file);
	self->logger = logger_new_instance();
	var(logger, self->logger);

	//datos de plataforma
	char* plataforma;
	if(config_has_property(global_config, "Plataforma"))
		plataforma = string_duplicate(config_get_string_value(global_config, "Plataforma"));
	else
		plataforma = string_duplicate(config_get_string_value(config, "Plataforma"));
	self->ippuerto_orquestador = plataforma;

	//datos de niveles y objetivos
	char** nombres_niveles = config_get_array_value(config, "Niveles");
	t_list* niveles = list_create();
	int i = 0;
	char* nombre_nivel = nombres_niveles[i];

	while(nombre_nivel != null){
		alloc(nivel, t_nivel);
		nivel->nombre = string_duplicate(nombre_nivel);
		nivel->objetivos = list_create();

		char* key_objetivos = string_from_format("Obj[%s]", nombre_nivel);
		char* objetivos = config_get_string_value(config, key_objetivos);

		logger_info(logger, "Nivel: %s", nivel->nombre);
		logger_info(logger, "Objetivos: %s", objetivos);
		int ii = 0;
		while(objetivos != null && objetivos[ii] != '\0'){
			char* objetivo = string_from_format("%c" , objetivos[ii]);
			list_add(nivel->objetivos, objetivo);
			ii++;
		}

		free(key_objetivos);

		list_add(niveles, nivel);

		i++;
		nombre_nivel = nombres_niveles[i];
	}
	self->niveles = niveles;

	logger_info(logger, "Log File:%s", log_file);
	logger_info(logger, "Log Level:%s", log_level);

	//liberamos recursos
	config_destroy(config);
	config_destroy(global_config);
	liberar_niveles(nombres_niveles);

	return self;
}
