/*
 * exception.h
 *
 *  Created on: Nov 25, 2013
 *      Author: pablo
 */


#include <unistd.h>

#include "jump.h"


#ifndef EXCEPTION_H_
#define EXCEPTION_H_

	/***************************************************************
	 * General exception handling
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


	//predefined exceptions
	#define RETRY_EXCEPTION 1


	#ifndef THREAD_H_

	/***************************************************************
	 * Mono-thread programs exception handling
	 ***************************************************************/

		//global variables
		process_status __try_ps;
		int excno;

		//try implementation macro
		#define SAVE_PS \
			excno = save_process_status(__try_ps)

		//throw implementation macro
		#define LOAD_PS(ex) \
			set_process_status(__try_ps, ex)

	#else

	/***************************************************************
	 * Multi-thread programs exception handling
	****************************************************************/

		//global variables
		tad_mutex __try_mutex = mutex_static;
		int __try_init = 0;

		//thread-local variables keys
		tad_thread_key __try_key;
		tad_thread_key __try_excno;

		//aux macros
		#define ptr_to_int(ptr) (int)(intptr_t)ptr
		#define int_to_ptr(value) (void*)(intptr_t)value

		//exception number implementation macro
		#define excno ptr_to_int(thread_get_variable(__try_excno))
		#define set_excno(value) thread_set_variable(__try_excno, int_to_ptr(value))

		//try implementation macro
		#define SAVE_PS \
			SAVE_PS_1(__LINE__)
		#define SAVE_PS_1(unique_id) \
			SAVE_PS_2(unique_id)
		#define SAVE_PS_2(unique_id) \
			SAVE_PS_3(__try_ps ## unique_id)
		#define SAVE_PS_3(ps) \
			mutex_close(&__try_mutex); \
			INIT_PS_THREAD_VARIABLES \
			process_status ps; \
			thread_set_variable(__try_key, &ps); \
			mutex_open(&__try_mutex); \
			set_excno(save_process_status(ps))

		#define INIT_PS_THREAD_VARIABLES \
			if(!__try_init){ \
				__try_init = 1; \
				__try_key = thread_create_variable(); \
				__try_excno = thread_create_variable(); \
			}

		//throw implementation macro
		#define LOAD_PS(ex) \
			LOAD_PS_1(ex, __LINE__)
		#define LOAD_PS_1(ex, unique_id) \
			LOAD_PS_2(ex, unique_id)
		#define LOAD_PS_2(ex, unique_id) \
			LOAD_PS_3(ex, __throw_ps ## unique_id)
		#define LOAD_PS_3(ex, ps) \
			do{ \
				process_status* ps = thread_get_variable(__try_key); \
				set_process_status(*ps, ex); \
			}while(0)

	#endif /* ifndef THREAD_H_ */
#endif /* ifndef EXCEPTION_H_ */