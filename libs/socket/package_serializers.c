/*
 * package_serializers.c
 *
 *  Created on: Oct 23, 2013
 *      Author: pablo
 */


#include <string.h>

#include "../vector/vector2.h"

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

tad_package* package_create_char_and_vector2(byte data_type, char c, vector2 v){
	int char_size = sizeof(char);
	int vector_size = sizeof(vector2);
	void* data = malloc(char_size + vector_size);
	memcpy(data, &c, char_size);
	void* vs = vector2_serialize(v);
	memcpy(data + char_size, vs, vector_size);
	free(vs);
	return package_create(data_type, char_size + vector_size, data);
}

void package_get_char_and_vector2(tad_package* package, char as_out c, vector2 as_out v){
	void* data = package_get_data(package);
	char* char_ptr = data;
	char char_ret = *char_ptr;
	void* vector_ptr = data + sizeof(char);
	vector2 vector_ret = vector2_deserialize(vector_ptr);
	set c = char_ret;
	set v = vector_ret;
}
