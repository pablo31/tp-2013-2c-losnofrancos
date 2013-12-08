#include "validate.h"

void validate_terminalSize() {
	void _dummy() {}
	t_anim* anim = anim_create(_dummy, _dummy, 0);
	if (anim == NULL) {
		printf("La terminal debe ser de mínimo 80x24.\n");
		exit(1);
	} else anim_destroy(anim);
}

void validate_paths(char** argv) {
	if (!files_directoryExists(argv[1])) {
		printf("El punto de montaje %s no existe.\n", argv[1]);
		exit(1);
	}

	if (!files_fileExists(argv[2])) {
		printf("El script %s no existe no existe.\n", argv[2]);
		exit(1);
	}
}
