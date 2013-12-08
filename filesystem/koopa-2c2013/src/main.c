#include "headers.h"

#define TEMP_DIRECTORY_NAME "/.koopa_tmp"
#define LINUX_LOG_NAME "linux_dump.txt"
#define GRASA_LOG_NAME "grasa_dump.txt"

void main_setPaths(char** argv);
void main_cleanPaths();
void main_generateDumps();
bool main_compareResults();
void main_goToAnimation();

static char *workingDirectory, *tmpDirectory;
static char *mountPoint, *script;
static char linuxLogName[MAX_PATH], grasaLogName[MAX_PATH];

extern bool win; extern char* dumpGrasa;

int main(int argc, char **argv) {
	clear();

	if (argc == 2 && string_equals(argv[1], "--version")) {
		printf("v1.1\n");
		exit(1);
	}
	if (argc != 3 && (argc != 4 || (argc == 4 && !string_equals(argv[3], "--text")))) {
		printf("Uso: ./koopa puntoMontaje script.sh [--text]\n");
		printf("puntoMontaje: Directorio con filesystem GRASA\n");
		printf("script.sh: Scripts que ejecuta operaciones sobre archivos\n");
		printf("--text: Modo texto\n");
		exit(1);
	}
	bool textMode = argc == 4;

	if (!textMode) validate_terminalSize();
	validate_paths(argv);
	main_setPaths(argv);

	files_resetDirectory(tmpDirectory);

	if (textMode) printf("Ejecutando operaciones...\n");
	magic_executeScript(tmpDirectory, mountPoint, script);

	files_fillPath(linuxLogName, workingDirectory, LINUX_LOG_NAME);
	files_fillPath(grasaLogName, workingDirectory, GRASA_LOG_NAME);

	if (textMode) printf("Generando dump en %s y %s...\n", LINUX_LOG_NAME, GRASA_LOG_NAME);
	unlink(linuxLogName); unlink(grasaLogName);
	main_generateDumps();

	win = main_compareResults();
	if (textMode) {
		if (win) printf("[OK] Los dumps coinciden...\n");
		else printf("[ERROR] Los dumps no coinciden...\n");
	}

	chdir(workingDirectory);
	if (!textMode) main_goToAnimation();

	main_cleanPaths();
	return !win;
}

void main_setPaths(char** argv) {
	char cwd[MAX_PATH];
	getcwd(cwd, sizeof(cwd));
	workingDirectory = files_getFullPath(cwd);
	mountPoint = files_getFullPath(argv[1]);
	script = files_getFullPath(argv[2]);
	tmpDirectory = string_duplicate(workingDirectory);
	string_append(&tmpDirectory, TEMP_DIRECTORY_NAME);
}

void main_cleanPaths() {
	free(workingDirectory); free(tmpDirectory);
	free(mountPoint); free(script);
}

void main_generateDumps() {
	magic_generateTree(tmpDirectory, linuxLogName);
	magic_generateMd5Log(tmpDirectory, linuxLogName);
	magic_generateTree(mountPoint, grasaLogName);
	magic_generateMd5Log(mountPoint, grasaLogName);
}

bool main_compareResults() {
	dumpGrasa = files_readFile(grasaLogName);
	char* dumpLinux = files_readFile(linuxLogName);

	bool win = string_equals(dumpLinux, dumpGrasa);

	free(dumpLinux); free(dumpGrasa);
	return win;
}

void main_goToAnimation() {
	t_anim* anim = anim_create(koopa_onInit, koopa_onUpdate, 85);
	anim_play(anim);
	anim_destroy(anim);
}
