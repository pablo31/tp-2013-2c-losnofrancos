/*
 * exception.h
 *
 *  Created on: Nov 25, 2013
 *      Author: pablo
 */


#include "jump.h"
//#include "../libs/thread/thread.h"


#ifndef EXCEPTION_H_
#define EXCEPTION_H_
	#ifndef THREAD_H_
		//exceptions for mono-thread programs

		process_status __try_ps;

		#define TRY \
			int excno = save_process_status(__try_ps); \
			if(!excno)
		#define CATCH(ex) \
			else if(excno == ex)
		#define CATCH_OTHER \
			else
		#define THROW(ex) \
			throw_process_status(__try_ps, ex)


	#else
		//exceptions for multi-thread programs

		tad_mutex __try_mutex = mutex_static;
		tad_thread_key __try_key;
		int __try_init = 0;

		void __initialize_try(){
			if(!__try_init){
				__try_init = 1;
				__try_key = thread_create_variable();
			}
		}

		#define TRY \
			TRY_impl1(__LINE__)
		#define CATCH(ex) \
			else if(excno == ex)
		#define CATCH_OTHER \
			else
		#define THROW(ex) \
			THROW_impl1(ex, __LINE__)

		#define TRY_impl1(line) \
			TRY_impl2(line)
		#define TRY_impl2(line) \
			TRY_impl3(__try_ps ## line)
		#define TRY_impl3(ps) \
			mutex_close(&__try_mutex); \
			__initialize_try(); \
			process_status ps; \
			thread_set_variable(__try_key, &ps); \
			mutex_open(&__try_mutex); \
			int excno = save_process_status(ps); \
			if(!excno)


		#define THROW_impl1(ex, line) \
			THROW_impl2(ex, line)
		#define THROW_impl2(ex, line) \
			THROW_impl3(ex, __throw_ps ## line)
		#define THROW_impl3(ex, ps) \
			process_status* ps = thread_get_variable(__try_key); \
			throw_process_status(*ps, ex);


	#endif /* ifndef THREAD_H_ */
#endif /* ifndef EXCEPTION_H_ */
