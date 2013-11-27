/*
 * common.h
 *
 *  Created on: Sep 12, 2013
 *      Author: pablo
 */

#include <stdint.h>
#include <stdlib.h>


//bienvenido al habitat natural de los macros mas aberrantes que jamas hayas visto


/***************************************
 * Std typedefs
 ***************************************/
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short int uint16;


/***************************************
 * Method visibility macros
 ***************************************/
#define public
#define private static

/***************************************
 * Misc macros
 ***************************************/
#define null NULL
#define string_equals(a, b) strcmp(a, b) == 0
#define max(a, b) (a > b ? a : b)

/***************************************
 * Class macro
 ***************************************/
	/* ejemplo de uso de class ***************
	 * class(nombre){
	 * 		tipo1 atributo1;
	 * 		tipo2 atributo2;
	 * 		....
	 * };
	 ****************************************/
	#define class(name) \
		class_impl1(name, __r_md_s_)

	//private macros
	#define class_impl1(name, struct_prefix) \
		class_impl2(name, struct_prefix ## name)
	#define class_impl2(name, struct_name) \
		struct struct_name; \
		typedef struct struct_name name; \
		struct struct_name

/***************************************
 * Object alloc macros
 ***************************************/
	/* ejemplo de uso de alloc (para objetos nuevos) *********************
	 * Sea:					typedef mi_estructura mi_tipo
	 * Si hacemos:			alloc(instancia, mi_tipo)
	 * Seria lo mismo que:	mi_tipo* instancia = malloc(sizeof(mi_tipo))
	 ********************************************************************/
	/* ejemplo de uso de alloc (para objetos preexistentes) **************
	 * Sea:					mi_tipo* mi_objeto
	 * Si hacemos:			alloc(mi_objeto)
	 * Seria lo mismo que:	mi_objeto = malloc(sizeof(mi_tipo))
	 ********************************************************************/
	/* ejemplo de uso de dealloc *****************************************
	 * Sea:					mi_tipo* obj
	 * Si hacemos:			dealloc(obj)
	 * Seria lo mismo que:	free(obj)
	 ********************************************************************/
	#define alloc(args...) \
		overload(alloc, args)
	#define dealloc(obj) \
		free(obj)

	//private macros
	#define alloc1(obj) \
		obj = malloc(sizeof(typeof(obj)))
	#define alloc2(obj, type) \
		type* obj = malloc(sizeof(type))

/***************************************
 * Short var type def macro
 ***************************************/
	/* ejemplo de uso de var ****************************************
	 * Sea:					mi_tipo objeto
	 * Si hacemos:			var(referencia, objeto)
	 * Seria lo mismo que:	mi_tipo referencia = objeto
	 ***************************************************************/
	#define var(name, obj) \
		typeof(obj) name = obj

/***************************************
 * Overload macros
 ***************************************/
	/* ejemplo de declaracion de funcion con overload ********************
	 * Implementamos las funciones:
	 * 		int mi_funcion(int a) { .... }
	 * 		int mi_funcion(int a, int b) { .... }
	 * 		int mi_funcion(int a, int b, int c) { .... }
	 * Y luego debemos definir el siguiente macro:
	 * 		#define mi_funcion(args...) overload(mi_funcion, args)
	 * Finalmente, podemos invocar a mi_funcion con sobrecarga:
	 * 		mi_funcion(a); mi_funcion(a, b); mi_funcion(a, b, c);
	 * Nota: se soporta sobrecarga para hasta 9 parametros.
	 ********************************************************************/
	#define overload(func, args...) \
		overload_numargs(func, args)
	#define overload_numargs(func, args...) \
		concat_numargs(func, args) (args)
	#define overload_hasargs(func, args...) \
		concat_hasargs(func, args) (args)

	//private macros
	#define concat_numargs(func, ...) \
		_concat_indirection(func, VA_NUM_ARGS(__VA_ARGS__))
	#define concat_hasargs(func, ...) \
		_concat_indirection(func, VA_HAS_ARGS(__VA_ARGS__))
	#define VA_NUM_ARGS(...) \
		VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
	#define VA_HAS_ARGS(...) \
		VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
	#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

/***************************************
 * Csharp style args out macros
 ***************************************/
	/* ejemplo de uso de out ********************
	 * Podemos definir funciones del tipo:
	 * void f(int as_out i){
	 * 		set i = 2;
	 * }
	 * Y podemos invocarlas haciendo:
	 * int i; f(out i);
	 *******************************************/
	#define set \
		*
	#define out \
		&
	#define as_out \
		*

/***************************************
 * List Foreach macro
 ***************************************/
	/* ejemplo de uso de foreach ************
	 * t_list* mi_lista;
	 * foreach(item, mi_lista, char*){
	 * 		printf("%s", item);
	 * }
	 ***************************************/
	#define foreach(item, list, type) \
		_foreach(item, list, type, __LINE__)

	//private macros
	#define _foreach(item, list, type, unique_id) \
		__foreach(item, list, type, _concat_indirection(__r_le, unique_id))
	#define __foreach(item, list, type, elem_ptr) \
		t_link_element* elem_ptr; \
		type item; \
		if(list->head != null) item = list->head->data; \
		if(list->head != null) \
		for(elem_ptr = list->head; elem_ptr != null; elem_ptr = elem_ptr->next, item = elem_ptr?elem_ptr->data:null)

/***************************************
 * List Remove Where macro
 ***************************************/
	/* ejemplo de uso de list_remove_where ***********************
	 * list_remove_where(lista, tipo* nombre_item, condicion)
	 * list_remove_where(lista, char* cadena, cadena == null)
	 ************************************************************/
	#define list_remove_where(list, item, predicate) \
		_list_remove_where(list, item, _concat_indirection(__r_md_lrw, __LINE__), predicate)

	//private macros
	#define _list_condition(name, item, predicate) \
		bool name (void* __r_md_ptr) { item = __r_md_ptr; return predicate; }
	#define _list_remove_where(list, item, func_name, predicate) \
		_list_condition(func_name, item, predicate); \
		list_remove_by_condition(list, func_name)


/***************************************
 * Private macros
 ***************************************/
	#define _concat_indirection(a, b) _concat(a, b)
	#define _concat(a, b) a ## b
