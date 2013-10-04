/*
 * common.h
 *
 *  Created on: Sep 12, 2013
 *      Author: pablo
 */

#include <stdint.h>

//method visibility
#define public
#define private static

//misc macros
#define null (void*)0

//common typedefs
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short int uint16;
typedef char string;

//max between two numbers
#define max(a, b) (a > b ? a : b)

//foreach loop
#define foreach(item, list, type) \
	t_link_element* __r_le; \
	type item; \
	if(list->head != null) item = list->head->data; \
	if(list->head != null) \
	for(__r_le = list->head; __r_le != null; __r_le = __r_le->next, item = __r_le?__r_le->data:null)
/* ejemplo de uso de foreach ************
 * t_list* mi_lista;
 * foreach(item, mi_lista, char*){
 * 		printf("%s", item);
 * }
 ***************************************/

//object alloc
#define alloc(name, type) \
	type* name = malloc(sizeof(type))
/* ejemplo de uso de alloc *******************************************
 * Sea:					typedef mi_estructura mi_tipo
 * Si hacemos:			alloc(instancia, mi_tipo)
 * Seria lo mismo que:	mi_tipo* instancia = malloc(sizeof(mi_tipo))
 ********************************************************************/
#define dealloc(obj) \
	free(obj)
/* ejemplo de uso de dealloc *****************************************
 * Sea:					mi_tipo* obj
 * Si hacemos:			dealloc(obj)
 * Seria lo mismo que:	free(obj)
 ********************************************************************/

//short-def var ref
#define var(name, obj) \
	typeof(obj) name = obj
#define ref(name, obj) \
	var(name, obj)
/* ejemplo de uso de var ********************
 * Sea:				tad_tipo* objeto
 * Si hacemos:		var(referencia, objeto)
 * O si hacemos:	ref(referencia, objeto)
 * Entonces:		referencia == objeto
 *******************************************/
