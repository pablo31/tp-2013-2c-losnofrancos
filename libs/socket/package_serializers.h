/*
 * package_serializers.h
 *
 *  Created on: Oct 23, 2013
 *      Author: pablo
 */

#ifndef PACKAGE_SERIALIZERS_H_
#define PACKAGE_SERIALIZERS_H_


#include "socket.h"
#include "../common.h"


tad_package* package_create_two_chars(byte data_type, char char1, char char2);
void package_get_two_chars(tad_package* package, char as_out char1, char as_out char2);

tad_package* package_create_char_and_vector2(byte data_type, char c, vector2 v);
void package_get_char_and_vector2(tad_package* package, char as_out c, vector2 as_out v);


#endif /* PACKAGE_SERIALIZERS_H_ */
