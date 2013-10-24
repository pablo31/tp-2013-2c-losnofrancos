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



#endif /* PACKAGE_SERIALIZERS_H_ */
