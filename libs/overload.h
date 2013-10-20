/*
 * variadic.h
 *
 *  Created on: Oct 17, 2013
 *      Author: pablo
 */

#ifndef OVERLOAD_H_
#define OVERLOAD_H_

/*
 * VARIADIC & FUNCTION OVERLOAD MACROS
 * Estos macros de aca abajo permiten implementar sobrecarga
 * Se pueden crear funciones con multiples definiciones (desde 0 hasta 9 parametros)
 * El codigo esta basado en
 * http://cplusplus.co.il/2010/07/17/variadic-macro-to-count-number-of-arguments/
 */


//indica la cantidad de argumentos de un __VA_ARGS__ (desde 0 hasta 5)
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
//devuelve 0 o 1 dependiendo si un __VA_ARGS__ es vacio o no, respectivamente
#define VA_HAS_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)


//concatena el nombre de una funcion con su cantidad de argumentos
#define concat_numargs(func, ...) \
		concat_indirection(func, VA_NUM_ARGS(__VA_ARGS__))
//concatena el nombre de una funcion con un 0 o 1 dependiendo de si recibe argumentos o no
#define concat_hasargs(func, ...) \
		concat_indirection(func, VA_HAS_ARGS(__VA_ARGS__))

#define overloaded(func, ...) concat_numargs(func, __VA_ARGS__) (__VA_ARGS__)


/*
 * Como crear una funcion con sobrecarga:
 * Por ejemplo, queremos crear la funcion max que puede recibir 1, 2 o 3 parametros.
 * Debemos implementar las tres funciones
 * int max(int a) { return a; }
 * int max(int a, int b) { return a>b?a:b;}
 * int max(int a, int b, int c) { return max(a, max(b,c)); }
 * Y luego debemos definir el macro
 * #define max(...) overloaded(max, __VA_ARGS__)
 * Finalmente, podemos invocar a cualquiera de las tres llamandolas simplemente max
 * max(a); max(a, b); max(a, b, c);
 */




//private macros
#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define concat_indirection(a, b) concat(a, b)
#define concat(a, b) a ## b


#endif /* OVERLOAD_H_ */
