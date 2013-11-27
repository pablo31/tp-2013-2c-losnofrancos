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

	//public macros
	#define TRY \
		PREPARE_TRY; \
		if(!excno || excno == RETRY_EXCEPTION)
	#define CATCH(ex) \
		else if(excno == ex)
	#define CATCH_OTHER \
		else
	#define THROW(ex) \
		THROW_impl(ex)
	#define RETRY \
		THROW(RETRY_EXCEPTION)


	//predefined exceptions
	#define RETRY_EXCEPTION 1


	#ifndef THREAD_H_

	/***************************************************************
	 * Mono-thread programs exception handling
	 ***************************************************************/

		//global variables
		process_status __try_ps;
		int excno;

		//prepare try implementation macro
		#define PREPARE_TRY \
			excno = save_process_status(__try_ps)

		//throw implementation macro
		#define THROW_impl(ex) \
			throw_process_status(__try_ps, ex)

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

		//prepare try implementation macro
		#define PREPARE_TRY \
			PREPARE_TRY_impl1(__LINE__)
		#define PREPARE_TRY_impl1(line) \
			PREPARE_TRY_impl2(line)
		#define PREPARE_TRY_impl2(line) \
			PREPARE_TRY_impl3(__try_ps ## line)
		#define PREPARE_TRY_impl3(ps) \
			mutex_close(&__try_mutex); \
			TRY_initialize \
			process_status ps; \
			thread_set_variable(__try_key, &ps); \
			mutex_open(&__try_mutex); \
			set_excno(save_process_status(ps))
		#define TRY_initialize \
			if(!__try_init){ \
				__try_init = 1; \
				__try_key = thread_create_variable(); \
				__try_excno = thread_create_variable(); \
			}

		//throw implementation macro
		#define THROW_impl(ex) \
			THROW_impl1(ex, __LINE__)
		#define THROW_impl1(ex, line) \
			THROW_impl2(ex, line)
		#define THROW_impl2(ex, line) \
			THROW_impl3(ex, __throw_ps ## line)
		#define THROW_impl3(ex, ps) \
			do{ \
				process_status* ps = thread_get_variable(__try_key); \
				throw_process_status(*ps, ex); \
			}while(0)

	#endif /* ifndef THREAD_H_ */
#endif /* ifndef EXCEPTION_H_ */
