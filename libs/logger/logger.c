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


private t_log* instance = null;


/**********************************************
 * SINGLETON **********************************
 **********************************************/

void logger_initialize(char* file, char* exe_name, char* log_level){
	t_log_level elv = log_level_from_string(log_level);
	instance = log_create(file, exe_name, 1, elv);
}
void logger_initialize(char* file, char* exe_name, char* log_level, int console){
	t_log_level elv = log_level_from_string(log_level);
	instance = log_create(file, exe_name, console, elv);
}

void logger_initialize_for_debug(char* file, char* exe_name){
	instance = log_create(file, exe_name, 1, LOG_LEVEL_DEBUG);
}

void logger_initialize_for_info(char* file, char* exe_name){
	instance = log_create(file, exe_name, 1, LOG_LEVEL_INFO);
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

	ret->needs_dealloc = 1;

	return ret;
}

tad_logger* logger_new_instance(){
	alloc(ret, tad_logger);
	ret->header = "";
	ret->needs_dealloc = 0;
	return ret;
}

void logger_dispose_instance(tad_logger* logger){
	if(logger->needs_dealloc) free(logger->header);
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

#define logger_implement_log_level_c(level_name) \
	void logger_##level_name##_val(tad_logger* logger, const char* text, va_list inargs){ \
		logger_write(logger, text, inargs, log_##level_name); } \
	void logger_##level_name(tad_logger* logger, const char* text, ...){ \
		va_list inargs; \
		va_start(inargs, text); \
		logger_##level_name##_val(logger, text, inargs); } \

logger_implement_log_level_c(trace);
logger_implement_log_level_c(info);
logger_implement_log_level_c(debug);
logger_implement_log_level_c(warning);
logger_implement_log_level_c(error);

#undef logger_implement_log_level_c
