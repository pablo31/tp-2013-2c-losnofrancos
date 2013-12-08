#ifndef MAGIC_H_
#define MAGIC_H_

	#include "../headers.h"

	void magic_executeScript(char* tmpDirectory, char* mountPoint, char* scriptPath);
	void magic_generateTree(char* path, char* pathToLog);
	void magic_generateMd5Log(char* path, char* pathToLog);

#endif
