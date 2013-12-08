#ifndef HEADERS_H_
#define HEADERS_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <string.h>
	#include <unistd.h>
	#include "commons/string.h"
	#include "commons/collections/list.h"

	#define new(TYPE) malloc(sizeof(TYPE))
	#define downcast(TYPE) \
		TYPE* this = (TYPE*) _this;
	#define string_equals(STR1, STR2) !strcmp(STR1, STR2)

#endif
