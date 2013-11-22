#include <string.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include "grasa.h"
#include <stdbool.h>
#include "filesystem.h"
#include "filesystem_operations.h"
#include "../libs/logger/logger.h"
#include "../libs/common/bitarray.h"
#include "../libs/common/string.h"
#include "../libs/common/collections/list.h"
#include <sys/stat.h>

#define TAMANIO_BLOQUE 		  4096
#define LOG_FILE              "grasa.log" //el archivo donde se va a guardar el log.
struct fuse_operations grasa_operations = { .mkdir = fs_mkdir, .unlink =
		fs_unlink, .rmdir = fs_rmdir, .read = fs_read, .open = fs_open, .write =
		fs_write, .opendir = fs_opendir, .readdir = fs_readdir, .create =
		fs_create, .getattr = fs_getattr, };

static char* file_name = NULL;
static int file_descriptor; // file descriptor del archivo de grasa
static uint file_size;      // el tamaño del archivo
struct grasa_header_t* header;
t_bitarray* bitmap;
GFile* nodos;
char* mmaped_file = NULL; // el array de chars que contiene el archivo via mmaped_file
uint bitmap_bytes_usados; // Tamaño archivo / blocksize / 8

tad_logger* logger;

sem_t mutex_nodos,mutex_mapa,mutex_datos;

static void set_file_name(char* name){	
	file_name = string_new();
	strcpy(file_name, name);	
}

static char* get_file_name(){
	return file_name;
}

static void iniciar_logger(char* exe_name) {
	//inicializamos el singleton logger
	logger_initialize_for_info(LOG_FILE, exe_name);
	logger = logger_new_instance("Filesystem");
}

void iniciar_semaforos(void){
	sem_init(&mutex_nodos,0,1);
	sem_init(&mutex_mapa,0,1);
	sem_init(&mutex_datos,0,1);
}

static void cerrar_logger() {
	logger_dispose();
}

void logear_header(struct grasa_header_t* header) {
	logger_info(logger, "Identificador:\t%s", header->grasa);
	logger_info(logger, "Versión:\t%i", header->version);
	logger_info(logger, "Bloque de inicio del bitmap:\t%i", header->blk_bitmap);
	logger_info(logger, "Tamaño del Bitmap:\t%i bloque(s)",
			header->size_bitmap);
	logger_info(logger, "Relleno lo leo pero no le doy pelota");
}
void logger_bitmap(t_bitarray* bitmap) {
	uint i, j;

	for (i = 0; i < bitmap_bytes_usados; ++i) {
		printf("%i) ", i);

		for (j = 0; j < 8; j++) //imprimo cada bit para verificar
			printf("%i", bitarray_test_bit(bitmap, (i * 8) + j));

		printf("\n");
	}
}

void loggear_nodos(GFile* nodos) {

	logger_info(logger, "Tabla de nodos");
	int i;
	for (i = 1; i < GFILEBYTABLE; ++i){
		if (nodos[i].state != 0) { // 0 = borrado.
			logger_info(logger, "Nodo %i)", i);
			switch (nodos[i].state) {
			case 1:
				logger_info(logger, "\tEstado: Archivo");
				break;
			case 2:
				logger_info(logger, "\tEstado: Directorio");
				break;
			default:
				//https://www.youtube.com/watch?v=iIs2iHvadzA
				break;
			}

			logger_info(logger, "\tNombre: %s", nodos[i].fname);
			logger_info(logger, "\tTamaño: %i", nodos[i].file_size);
			logger_info(logger, "\tCreacion: %i", nodos[i].c_date);
			logger_info(logger, "\tModificacion: %i", nodos[i].m_date);
			logger_info(logger, "\tPadre: %i", nodos[i].parent_dir_block);
		}
	}
}

void cargar_header() {
	header = malloc(sizeof(struct grasa_header_t));

	memcpy(header, mmaped_file, sizeof(struct grasa_header_t));
	logear_header(header);
}

static void cargar_bitmap() {
	bitmap_bytes_usados = file_size / TAMANIO_BLOQUE / 8;
	logger_info(logger, "Cantidad de bytes usados en bitmap :%i",
			bitmap_bytes_usados);

	char* str_bitmap = string_substring(mmaped_file, TAMANIO_BLOQUE,
			8 * bitmap_bytes_usados);
	bitmap = bitarray_create(str_bitmap, sizeof(char*) * bitmap_bytes_usados);
	logger_bitmap(bitmap);
}

static void cargar_nodos() {
    uint inicio_nodos = (header->size_bitmap + 1) * TAMANIO_BLOQUE;
    nodos = (GFile*)(mmaped_file + inicio_nodos);
    nodos = &nodos[-1];

    loggear_nodos(nodos);
}

void liberar_recursos() {
	if (mmaped_file != NULL ) {
		munmap(mmaped_file, file_size);
		free(mmaped_file);
	}

	free(file_name);
	free(bitmap);

	if (header != NULL ) {
		free(header->padding);
		free(header->grasa);
		free(header);
	}

	cerrar_logger();
}

void cargar_configuracion_grasa(int argc, char *argv[]) {
	set_file_name(argv[1]);

	file_descriptor = open(get_file_name(), O_RDWR);

	struct stat status;
	fstat(file_descriptor, &status);
	file_size = status.st_size;

	logger_info(logger, "Tamaño del archivo grasa: %i.", file_size);
	mmaped_file = mmap(NULL, status.st_size, PROT_WRITE, MAP_SHARED,
			file_descriptor, 0);

	if (mmaped_file == MAP_FAILED ) {
		logger_error(logger,
				"Error al iniciar mmaped_file con el archivo %s. '%s' ",
				get_file_name(), strerror(errno));
		cerrar_logger();
		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
	}
}

int main(int argc, char *argv[]) {
	iniciar_logger(argv[0]);
	iniciar_semaforos();
	cargar_configuracion_grasa(argc, argv);

	// saco el archivo de grasa de la lista de argumentos
	int i;
	for(i=2;i<argc;i++){
		argv[i-1] = argv[i];
	}

	argv[argc - 1] = NULL;
	argc--;

	// En vez de sacar el primer elemento envio los valores modificados a fuse
	//struct fuse_args args = FUSE_ARGS_INIT(argc,argv);
	cargar_header();
	cargar_bitmap();
	cargar_nodos();

	int fuse_retorno = fuse_main(argc, argv, &grasa_operations,
			NULL );

	logger_info(logger, "Fin de fuse_main - %s.",
			(fuse_retorno == 0) ? "Finalizacion correcta" : "Error");

	return EXIT_SUCCESS;
}

