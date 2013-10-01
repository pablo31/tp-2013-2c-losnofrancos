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

void signal_declare_handler(int signal_id, void* function, int numargs, ...);
void signal_dispose_all();


#endif /* SIGNAL_H_ */
