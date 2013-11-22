/*
 * notifier.c
 *
 *  Created on: Oct 28, 2013
 *      Author: pablo
 */

#include <unistd.h>
#include <sys/inotify.h>

#include "notifier.h"

tad_notifier* notifier_create(char* file_path){
	alloc(self, tad_notifier);
	int nfd = inotify_init();
	self->watch_fd = inotify_add_watch(nfd, file_path, IN_MODIFY);
	self->fd = nfd;
	return self;
}

void notifier_wait_for_modification(tad_notifier* self){
	struct inotify_event event;
	read(self->fd, &event, sizeof(event)); //bloqueante
}

void notifier_dispose(tad_notifier* self){
	int nfd = self->fd;
	inotify_rm_watch(nfd, self->watch_fd);
	close(nfd);
}

int __notifier_get_fd(void* self){
	return ((tad_notifier*)self)->fd;
}
void __notifier_dispose(void* self){
	notifier_dispose(self);
}
