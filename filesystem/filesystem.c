#include <string.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "grasa.h"
#include <stdbool.h>
#include <math.h>
#include "filesystem.h"
#include "filesystem_operations.h"
#include "../libs/logger/logger.h"
#include "../libs/common/bitarray.h"
#include "../libs/common/string.h"

#define TAMANIO_BLOQUE 		  4096
#define LOG_FILE              "grasa.log"

struct fuse_operations grasa_operations = {
  .mkdir = fs_mkdir,
  .unlink = fs_unlink,
  .rmdir = fs_rmdir,
  .read = fs_read,
  .write = fs_write,
  .opendir = fs_opendir,
  .create = fs_create,
};


static char* file_name   = NULL;
static int   file_descriptor; // file descriptor del archivo de grasa
static uint  file_size;      // el tamaño del archivo
struct grasa_header_t* header;
char* mmaped_file = NULL;// el array de chars que contiene el archivo via mmaped_file
t_bitarray* grasa_bitmap;
tad_logger* logger;

static void iniciar_logger(char* exe_name){
	//inicializamos el singleton logger
	logger_initialize_for_info(LOG_FILE, exe_name);
	logger = logger_new_instance("Filesystem");
}

static void cerrar_logger(){
	logger_dispose();
}

void logear_header(struct grasa_header_t* header){
	logger_info(logger, "Identificador:\t%s", header->grasa);
 	logger_info(logger, "Versión:\t%i", header->version);
 	logger_info(logger, "Bloque de inicio del bitmap:\t%i", header->blk_bitmap);	
	logger_info(logger, "Tamaño del Bitmap:\t%i bloque(s)", header->size_bitmap);
	logger_info(logger, "Relleno lo leo pero no le doy pelota");
}

struct grasa_header_t* cargar_header(){
 	struct grasa_header_t* header = malloc(sizeof(struct grasa_header_t));

	memcpy(header, mmaped_file, sizeof(struct grasa_header_t));
	logear_header(header);

 	return header;
}

static t_bitarray* cargar_bitmap(uint32_t cantidad_bloques){
	uint tamanio_total =  cantidad_bloques * TAMANIO_BLOQUE ;
	logger_info(logger,"Tamaño total del bitmap:%i",tamanio_total);
	t_bitarray* bitmap = malloc(sizeof(t_bitarray) * tamanio_total);

	return bitmap;
}



static void cargar_configuracion_grasa(int argc, char *argv[]){
	if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')){
		logger_error(logger, "uso:\n\t./filesystem.sh [opciones de FUSE] archivo_grasa directorio_donde_montar");
		cerrar_logger();
	    exit(EXIT_FAILURE);
	}

	file_name = string_new();
	strcpy(file_name, argv[1]);

	file_descriptor = open(file_name, O_RDONLY);

	struct stat status;
    fstat(file_descriptor, &status);
    file_size = status.st_size;
    
 	mmaped_file = mmap(NULL, status.st_size, PROT_READ, MAP_SHARED, file_descriptor, 0);	

 	if (mmaped_file == MAP_FAILED){
 		logger_error(logger,"Error al iniciar mmaped_file con el archivo %s. '%s' ", file_name, strerror(errno));
 		cerrar_logger();
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
 	}
}

/*
static void liberar_recursos(){
	if(mmaped_file != NULL){
		munmap(mmaped_file , file_size);
		free(mmaped_file);
	}

	free(file_name);
}
*/

int main(int argc, char *argv[]){
	iniciar_logger(argv[0]);
	cargar_configuracion_grasa(argc, argv);

	// saco el archivo de grasa de la lista de argumentos 
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;

	struct fuse_args args = FUSE_ARGS_INIT(argc,argv);
 	header = cargar_header(); 
 	grasa_bitmap = cargar_bitmap(header->size_bitmap);
    
    fuse_main(args.argc, args.argv, &grasa_operations, NULL);

    //liberar_recursos();
	
	return EXIT_SUCCESS;
}
