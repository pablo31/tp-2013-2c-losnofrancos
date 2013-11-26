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

		process_status __r_try_ps;

		#define TRY \
			int exno = save_process_status(__r_try_ps); \
			if(!exno)
		#define CATCH(ex) \
			else if(exno == ex)
		#define CATCH_OTHER \
			else
		#define THROW(ex) \
			throw_process_status(__r_try_ps, ex)


	#else
		//exceptions for multi-thread programs

		tad_mutex try_mutex = mutex_static;
		tad_thread_key try_ps_key;
		int trys_initialized = 0;

		void initialize_trycatch(){
			if(!trys_initialized){
				trys_initialized = 1;
				try_ps_key = thread_create_variable();
			}
		}

		#define TRY \
			mutex_close(&try_mutex); \
			initialize_trycatch(); \
			process_status ps; \
			thread_set_variable(try_ps_key, &ps); \
			mutex_open(&try_mutex); \
			int exno = save_process_status(ps); \
			if(!exno)
		#define CATCH(ex) \
			else if(exno == ex)
		#define CATCH_OTHER \
			else
		#define THROW(ex) \
			process_status* ps = thread_get_variable(try_ps_key); \
			throw_process_status(*ps, ex); \


	#endif /* ifndef THREAD_H_ */
#endif /* ifndef EXCEPTION_H_ */
