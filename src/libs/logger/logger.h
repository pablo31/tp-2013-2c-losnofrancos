/*
 * newlogger.h
 *
 *  Created on: May 28, 2013
 *      Author: pablo
 */

#ifndef NEWLOGGER_H_
#define NEWLOGGER_H_


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

//Instance operations
void logger_trace(tad_logger* logger, const char* text, ...);
void logger_info(tad_logger* logger, const char* text, ...);
void logger_debug(tad_logger* logger, const char* text, ...);
void logger_warning(tad_logger* logger, const char* text, ...);
void logger_error(tad_logger* logger, const char* text, ...);


#endif /* NEWLOGGER_H_ */
