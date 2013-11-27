/*
 * jump.h
 *
 *  Created on: Nov 25, 2013
 *      Author: pablo
 */

#ifndef JUMP_H_
#define JUMP_H_


#include <setjmp.h>

//Methods
#define save_process_status(status) setjmp(status)
#define load_process_status(status) longjmp(status, 1)
#define set_process_status(status, result) longjmp(status, result)

//Typedef
typedef jmp_buf process_status;

//Error Management Block Definition
#define DECLARE_ERROR_MANAGER \
	process_status __r_md_ps; \
	if(save_process_status(__r_md_ps))


#endif /* JUMP_H_ */
