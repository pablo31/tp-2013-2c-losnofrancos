/*
 * package_serializers.c
 *
 *  Created on: Oct 23, 2013
 *      Author: pablo
 */

#include "package_serializers.h"

tad_package* package_create_two_chars(byte data_type, char char1, char char2){
	int size = 2 * sizeof(char);
	char* ptr = malloc(size);
	*ptr = char1;
	ptr += 1;
	*ptr = char2;
	return package_create(data_type, size, ptr);
}
void package_get_two_chars(tad_package* package, char as_out char1, char as_out char2){
	char* ptr = package_get_data(package);
	set char1 = *ptr;
	ptr += 1;
	set char2 = *ptr;
}
