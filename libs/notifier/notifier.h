/*
 * notifier.h
 *
 *  Created on: Oct 28, 2013
 *      Author: pablo
 */

#ifndef NOTIFIER_H_
#define NOTIFIER_H_

#include "../common.h"

class(tad_notifier){
	int fd;
	int watch_fd;
};


/***************************************************************
 * Public methods
 ***************************************************************/

//Crea una instancia notifier
tad_notifier* notifier_create(char* file_path);
//Bloquea la ejecucion del programa hasta que se produzca una modificacion en el archivo
void notifier_wait_for_modification(tad_notifier* notifier);
//Libera los recursos del notifier
void notifier_dispose(tad_notifier* notifier);


/***************************************************************
 * Notifier-Multiplexor methods
 ***************************************************************/

//Devuelve el FD del notifier (uso exclusivo del multiplexor)
int __notifier_get_fd(void* notifier);
//Libera los recursos del notifier (uso exclusivo del multiplexor)
void __notifier_dispose(void* notifier);


#endif /* NOTIFIER_H_ */
