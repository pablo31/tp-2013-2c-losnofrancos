/*
 * signal.h
 *
 *  Created on: Sep 16, 2013
 *      Author: pablo
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <signal.h>

#include "../command/command.h"

//Asocia una senal a una funcion manejadora
void signal_declare_handler(int signal_id, void* function, int numargs, ...);
//Libera todos los recursos y funciones asociadas a las senales
void signal_dispose_all();

//handler dynamic declaration
#define signal_dynamic_handler(signal_id, call) \
	void __r_md_shf_##signal_id(void){ \
		call; \
	} \
	signal_declare_handler(signal_id, __r_md_shf_##signal_id, 0);


#endif /* SIGNAL_H_ */
