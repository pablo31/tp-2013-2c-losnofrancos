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
		//exceptions for multi-thread programs (NOT WORKING)

		typedef struct s_thread_try{
			tad_thread		t;
			process_status	ps;
		} thread_try;

		thread_try	thread_trys[256];
		int			thread_trys_length = 0;

		static void expand_tta(){
			thread_trys_length++;
		}

		static thread_try* create_tt(tad_thread t){
			expand_tta();
			thread_trys[thread_trys_length].t = t;
			return &(thread_trys[thread_trys_length]);
		}

		thread_try* get_tt(tad_thread t){
			int i;
			for(i = 0; i < thread_trys_length; i++)
				if(thread_trys[i].t == t)
					return thread_trys + i;
			return create_tt(t);
		}


		#define TRY \
			tad_thread t = thread_self(); \
			thread_try* tt = get_tt(t); \
			int exno = save_process_status(tt->ps); \
			if(!exno)
		#define CATCH(ex) \
			else if(exno == ex)
		#define CATCH_OTHER \
			else
		#define THROW(ex) \
			tad_thread t = thread_self(); \
			thread_try* tt = get_tt(t); \
			throw_process_status(tt->ps, ex); \


	#endif /* ifndef THREAD_H_ */
#endif /* ifndef EXCEPTION_H_ */
