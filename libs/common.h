/*
 * common.h
 *
 *  Created on: Sep 12, 2013
 *      Author: pablo
 */

#include <stdint.h>
#include <stdlib.h>

/***************************************
 * METHOD VISIBILITY *******************
 ***************************************/
#define public
#define private static


/***************************************
 * MISC MACROS *************************
 ***************************************/
#define null (void*)0
#define string_equals(a, b) strcmp(a, b) == 0
#define max(a, b) (a > b ? a : b)

/***************************************
 * COMMON TYPEDEFS *********************
 ***************************************/
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short int uint16;
typedef char string;


/***************************************
 * HI-LEVEL MACROS *********************
 ***************************************/

//short class typedef
#define class(name) \
	struct __r_md_s_ ## name; \
	typedef struct __r_md_s_ ## name name; \
	struct __r_md_s_ ## name

//object alloc
#define alloc(obj, type) \
	type* obj = malloc(sizeof(type))
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
#define ralloc(obj) \
		obj = malloc(sizeof(typeof(obj)))

//short-def var ref
#define var(name, obj) \
	typeof(obj) name = obj
	/* ejemplo de uso de var ********************
	 * Sea:				tad_tipo* objeto
	 * Si hacemos:		var(referencia, objeto)
	 * Entonces:		referencia == objeto
	 *******************************************/

//csharp style args out
#define set *
#define out &
#define as_out *
	/* ejemplo de uso de out ********************
	 * Podemos definir funciones del tipo:
	 * void f(int as_out i){
	 * 		set i = 2;
	 * }
	 * Y podemos invocarlas haciendo:
	 * int i; f(out i);
	 *******************************************/





/***************************************
 * LIST MACROS *************************
 ***************************************/

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

//remove where
#define list_remove_where(list, item, predicate) \
	_list_remove_where(list, item, _concat_indirection(__r_md_lrw, __LINE__), predicate)








/***************************************
 * PRIVATE MACROS **********************
 ***************************************/

#define _concat_indirection(a, b) _concat(a, b)
#define _concat(a, b) a ## b

#define _list_condition(name, item, predicate) \
	bool name (void* __r_md_ptr) { item = __r_md_ptr; return predicate; }
#define _list_remove_where(list, item, func_name, predicate) \
	_list_condition(func_name, item, predicate); \
	list_remove_by_condition(list, func_name)
