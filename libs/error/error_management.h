/*
 * bifurcation.h
 *
 *  Created on: Sep 12, 2013
 *      Author: pablo
 */

#ifndef ERROR_MANAGEMENT_H_
#define ERROR_MANAGEMENT_H_

/*
 * WARNING!
 * DO NOT LOOK AT, TOUCH, INGEST, OR ENGAGE IN CONVERSATION WITH ANY SUBSTANCES LOCATED BEYOND THIS POINT
 *
 * By Pablo D;
 *
 * Desaventurados:
 * Como (por motivos desconocidos) no se puede encapsular el comportamiento
 * de jmp en metodos con nombres mas lindos, me decidi por laburar con
 * define tanto para las funciones setjmp como longjmp.
 * En fin. Dado que en general la gente tiende a asustarse cuando uno juega
 * con la call stack (no se que le ven de malo), en algun momento hare un doc
 * para que sepan como se usan estas cosas de aca abajo.
 * Bien, dejando de lado la sanata, lo unico importante de este texto es lo
 * que viene a continuacion:
 *
 * NO TOQUEN NADA!!!
 * Si bien en un principio lo que pasa aca adentro parece oscuro, el
 * funcionamiento es totalmente lineal y muy simple. Pero no hace falta que
 * decoren mis 3 lineas de codigo (que funcionan perfectamente) con sus
 * pinceladas de creatividad.
 * Pero no me malinterpreten! No quiero reprimir su lado creativo! Si
 * quieren pintar, los invito a que lo hagan en la parte de File System :D
 */


#include <setjmp.h>

//Methods
#define save_process_status(status) setjmp(status)
#define load_process_status(status) longjmp(status, 1)

//Typedef
typedef jmp_buf process_status;

//Error Management Block Definition
#define DECLARE_ERROR_MANAGER \
	process_status __r_md_ps; \
	if(save_process_status(__r_md_ps))


//thats all u gonna need! (;




//Try Catch Block (beta, do not use it)
#define TRY \
	jmp_buf __r_md_tjb; \
	int __ex_num = setjmp(__r_md_tjb); \
	if(!__ex_num)
#define CATCH(ex) \
	else if(__ex_num == ex)
#define CATCH_OTHER \
	else
#define THROW(ex) \
	longjmp(__r_md_tjb, ex)


#endif /* ERROR_MANAGEMENT_H_ */
