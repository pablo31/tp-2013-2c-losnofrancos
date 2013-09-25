/*
 * common.h
 *
 *  Created on: Sep 12, 2013
 *      Author: pablo
 */

#include <stdint.h>

#define public
#define private static

#define null (void*)0

typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short int uint16;

//foreach(item_name, list_name, item_type*){ ... }
#define foreach(item, list, type) \
	t_link_element* __r_le; type item = list->head->data; \
	for(__r_le = list->head; __r_le != null; __r_le = __r_le->next, item = __r_le?__r_le->data:null)

//max(num_a, num_b)
#define MAX(a, b) (a > b ? a : b)
