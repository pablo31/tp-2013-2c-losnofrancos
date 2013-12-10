/*
 * exception.h
 *
 *  Created on: Nov 25, 2013
 *      Author: pablo
 */


#include <unistd.h>
#include <errno.h>

#include "jump.h"


#ifndef EXCEPTION_H_
#define EXCEPTION_H_

	#define tls_var __thread
	extern tls_var process_status __try_ps;
	extern tls_var int excno;

	/***************************************************************
	 * Exception handling public macros
	 ***************************************************************/

	//java-style macros
	#define TRY \
		SAVE_PS; \
		if(!excno || excno == RETRY_EXCEPTION)
	#define CATCH(ex) \
		else if(excno == ex)
	#define CATCH_OTHER \
		else
	#define THROW(ex) \
		LOAD_PS(ex)

	//ruby-style macros
	#define RETRY \
		THROW(RETRY_EXCEPTION)

	//vb-style macros
	#define ON \
		SAVE_PS; \
		if(excno && excno != RETRY_EXCEPTION)
	#define EXCEPTION(ex) \
		if(excno == ex)
	#define OTHER_EXCEPTION \
		else


	/***************************************************************
	 * Exception handling private macros
	 ***************************************************************/

	//try implementation macro
	#define SAVE_PS \
		excno = save_process_status(__try_ps)

	//throw implementation macro
	#define LOAD_PS(ex) \
		set_process_status(__try_ps, ex)


	/***************************************************************
	 * Predefined exceptions
	 ***************************************************************/

	#define RETRY_EXCEPTION 1


#endif /* ifndef EXCEPTION_H_ */
