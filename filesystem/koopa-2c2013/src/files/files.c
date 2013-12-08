#include "files.h"

char* files_getFullPath(char* path)
	{ return realpath(path, (void*) NULL); }
void files_fillPath(char* path, char* directory, char* fileName)
	{ sprintf(path, "%s/%s", directory, fileName); }

bool files_fileExists(char* path) {
	FILE* file = fopen(path, "r");
	bool exists = file != NULL;
	fclose(file);
	return exists;
}

bool files_directoryExists(char* path) {
	DIR* dir = opendir(path);
	bool exists = dir;
	closedir(dir);
	return exists;
}

void files_resetDirectory(char* path) {
	char command[MAX_PATH];
	sprintf(command, "rm -rf %s", path);
	system(command);
	mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
}

t_list* files_getEntriesOfDirectory(char* path) {
	t_list* list = list_create();

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path)) != NULL) {
		while((ent = readdir(dir)) != NULL) {
			char* name = ent->d_name;
			if (!string_equals(name, ".") && !string_equals(name, ".."))
				list_add(list, string_duplicate(name));
		}
		closedir(dir);
	}

	return list;
}

char* files_readFile(char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) return NULL;

	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char* text = malloc(size + 1);
	fread(text, size, sizeof(char), file);
	fclose(file);
	text[size] = '\0';

	return text;
}
