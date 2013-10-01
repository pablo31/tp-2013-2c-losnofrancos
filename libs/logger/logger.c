/*
 * newlogger.c
 *
 *  Created on: May 28, 2013
 *      Author: pablo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/string.h"
#include "../common/log.c"

#include "../common.h"

#include "logger.h"




private const bool LOGEAR_EN_CONSOLA = true; //Si el log se muestra o no por pantalla.
//private const t_log_level NIVEL_MINIMO_LOG = LOG_LEVEL_DEBUG; //El nivel minimo de importancia que se guarda en el log.


/**********************************************
 * SINGLETON **********************************
 **********************************************/

private t_log* instance = null;

void logger_initialize_for_debug(char* file, char* exe_name){
	instance = log_create(file, exe_name, LOGEAR_EN_CONSOLA, LOG_LEVEL_DEBUG);
}

void logger_initialize_for_info(char* file, char* exe_name){
	instance = log_create(file, exe_name, LOGEAR_EN_CONSOLA, LOG_LEVEL_INFO);
}

void logger_dispose(){
	log_destroy(instance);
}


/**********************************************
 * INSTANCE INITIALIZATION ********************
 **********************************************/

tad_logger* logger_new_instance(const char* header, ...){
	alloc(ret, tad_logger);

	va_list args;
	va_start(args, header);
	char* formatted_header = string_from_vformat(header, args);

	if(strlen(formatted_header) == 0)
		ret->header = formatted_header;
	else{
		char* final_header = string_from_format("%s - ", formatted_header);
		ret->header = final_header;
		free(formatted_header);
	}

	return ret;
}

void logger_dispose_instance(tad_logger* logger){
	free(logger->header);
	dealloc(logger);
}


/**********************************************
 * LOG LEVELS *********************************
 **********************************************/

private void logger_write(tad_logger* logger,
		const char* text, va_list inargs,
		void (*function)(t_log*, const char*, ...)){

	char* formatted_text = string_from_vformat(text, inargs);
	char* final_text = string_from_format("%s%s", logger->header, formatted_text);

	function(instance, final_text);

	va_end(inargs);
	free(final_text);
	free(formatted_text);
}

#define logger_implement_log_level(level_name) \
	void logger_##level_name##_val(tad_logger* logger, const char* text, va_list inargs){ \
		logger_write(logger, text, inargs, log_##level_name); } \
	void logger_##level_name(tad_logger* logger, const char* text, ...){ \
		va_list inargs; \
		va_start(inargs, text); \
		logger_##level_name##_val(logger, text, inargs); } \

logger_implement_log_level(trace);
logger_implement_log_level(info);
logger_implement_log_level(debug);
logger_implement_log_level(warning);
logger_implement_log_level(error);
