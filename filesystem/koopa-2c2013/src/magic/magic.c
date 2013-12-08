#include "magic.h"

void magic_executeScript(char* tmpDirectory, char* mountPoint, char* scriptPath) {
	chdir(tmpDirectory);
	system(scriptPath);
	chdir(mountPoint);
	system(scriptPath);
}

void magic_generateTree(char* path, char* pathToDump) {
	char command[MAX_PATH];
	chdir(path);
	sprintf(command, "tree . > %s", pathToDump);
	system(command);
}

void magic_generateMd5Log(char* path, char* pathToDump) {
	t_list* entries = files_getEntriesOfDirectory(path);
	bool _sortByName(char* str1, char* str2) {
		return strcmp(str1, str2) < 0;
	}
	list_sort(entries, (void*) _sortByName);

	char command[MAX_PATH];
	sprintf(command, "touch %s", pathToDump);
	system(command);
	void _printMd5(char* fileName) {
		chdir(path);
		if (files_directoryExists(fileName)) {
			char* fullPath = files_getFullPath(fileName);
			magic_generateMd5Log(fullPath, pathToDump);
			free(fullPath);
		} else {
			sprintf(command, "md5sum %s >> %s", fileName, pathToDump);
			system(command);
		}
	}
	system(command);
	list_iterate(entries, (void*) _printMd5);

	list_destroy_and_destroy_elements(entries, free);
}
