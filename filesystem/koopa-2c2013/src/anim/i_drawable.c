#include "i_drawable.h"

int drawable_width(void* _this) {
	downcast(i_drawable)

	int width = 0;
	void _calculateMaxWidth(char* line) {
		int newWidth = strlen(line);
		if (newWidth > width)
			width = newWidth;
	}
	string_split_and_iterate(this->text, "\n", _calculateMaxWidth);

	return width;
}

int drawable_height(void* _this) {
	downcast(i_drawable)

	int newLines = 0;
	for(int i=0; this->text[i] != '\0'; i++)
		if (this->text[i] == '\n') newLines++;

	return 1 + newLines;
}

long drawable_getTime() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;
}
