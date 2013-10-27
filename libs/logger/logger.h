/*
 * newlogger.h
 *
 *  Created on: May 28, 2013
 *      Author: pablo
 */

#ifndef NEWLOGGER_H_
#define NEWLOGGER_H_

#include <stdarg.h>

struct s_logger{
	char* header;
};
typedef struct s_logger tad_logger;


//Singleton initialization & disposal
void logger_initialize_for_debug(char* file, char* exe_name);
void logger_initialize_for_info(char* file, char* exe_name);
void logger_dispose();

//Instance initialization & disposal
tad_logger* logger_new_instance(const char* header, ...);
void logger_dispose_instance(tad_logger* logger);

//Log level implementation
#define logger_implement_log_level_h(level_name) \
	void logger_##level_name##_val(tad_logger* logger, const char* text, va_list inargs); \
	void logger_##level_name(tad_logger* logger, const char* text, ...)

//Log levels
logger_implement_log_level_h(trace);
logger_implement_log_level_h(info);
logger_implement_log_level_h(debug);
logger_implement_log_level_h(warning);
logger_implement_log_level_h(error);

#undef logger_implement_log_level_h

//Another way to log
#define logger_log(logger, level_name, ...) \
		logger_##level_name(logger, __VA_ARGS__)


#endif /* NEWLOGGER_H_ */
