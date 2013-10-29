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


tad_notifier* notifier_create(char* file_path);
void notifier_wait_for_modification(tad_notifier* notifier);
void notifier_dispose(tad_notifier* notifier);

int __notifier_get_fd(void* notifier);
void __notifier_dispose(void* notifier);


#endif /* NOTIFIER_H_ */
