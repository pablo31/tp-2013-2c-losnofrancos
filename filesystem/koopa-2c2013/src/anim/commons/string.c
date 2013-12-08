#include "string.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

char* string_duplicate(char* original) {
	return strdup(original);
}

char* string_repeat(char character, int count) {
	char *text = calloc(count + 1, 1);
	for (int i = 0; i < count; ++i) {
		text[i] = character;
	}
	return text;
}

char* string_replaceChar(char* string, char find, char replace) {
	for(int i=0; i < strlen(string); i++)
		if (string[i] == find)
			string[i] = replace;

	return string;
}

void string_append(char** original, char* string_to_add) {
	*original = realloc(*original, strlen(*original) + strlen(string_to_add) + 1);
	strcat(*original, string_to_add);
}

char** string_split(char* text, char* separator) {
	char** substrings = NULL;
	int size = 0;

	char *text_to_iterate = string_duplicate(text);
	char *token = NULL, *next = NULL;
	token = strtok_r(text_to_iterate, separator, &next);

	while (token != NULL) {
		size++;
		substrings = realloc(substrings, sizeof(char*) * size);
		substrings[size - 1] = string_duplicate(token);
		token = strtok_r(NULL, separator, &next);
	}

	size++;
	substrings = realloc(substrings, sizeof(char*) * size);
	substrings[size - 1] = NULL;

	free(text_to_iterate);
	return substrings;
}

void string_split_and_iterate(char* text, char* separator, void (*closure)(char*)) {
	char* copy = string_duplicate(text);

	char* line = strtok(copy, "\n");
	for(int i=0; line != NULL; i++, line = strtok(NULL, "\n"))
		closure(line);

	free(copy);
}
