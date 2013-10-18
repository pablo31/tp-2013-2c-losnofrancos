/*
 * variadic.h
 *
 *  Created on: Oct 17, 2013
 *      Author: pablo
 */

#ifndef VARIADIC_H_
#define VARIADIC_H_

/*
 * VARIADIC MACROS
 * Estos macros de aca abajo te permiten implementar sobrecarga
 * Se pueden crear funciones con multiples definiciones (desde 1 hasta 5 parametros)
 * El codigo no es mio, lo saque de
 * http://cplusplus.co.il/2010/07/17/variadic-macro-to-count-number-of-arguments/
 * Estos flacos se sarpan s:
 */

//va_args args-count macro
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,N,...) N

//overload
#define macro_dispatcher(func, ...) \
            macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) \
            macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) \
            func ## nargs



////va-args args-count macro (can return 0)
//#define VA_NUM_ARGS(...)
//    (sizeof(#__VA_ARGS__) == sizeof("")
//     ? 0 : VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1))


#endif /* VARIADIC_H_ */
