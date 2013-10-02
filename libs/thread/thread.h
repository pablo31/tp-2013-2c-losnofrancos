/*
 * thread.h
 *
 *  Created on: May 9, 2013
 *      Author: pablo
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

#include "../command/command.h"

struct s_thread{
	pthread_t thread;
};
typedef struct s_thread tad_thread;

//Creation
tad_thread* thread_begin(void* function, int numargs, ...);
void thread_free_begin(void* function, int numargs, ...);

//Disposal
void thread_join(tad_thread* thread);


/*#define thread_begin_calling(call, thread) \
	void __r_md_th##thread(void){ \
		call; \
	} \
	thread = thread_begin(__r_md_th##thread, 0);*/

#endif /* THREAD_H_ */
