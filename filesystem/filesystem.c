#include <string.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "grasa.h"
#include <stdbool.h>
#include "filesystem_operations.h"
#include "../libs/logger/logger.h"
#include "../libs/common/string.h"

#define TAMANIO_IDENTIFICADOR 5
#define TAMANIO_VERSION       4
#define TAMANIO_PTR_BITMAP    sizeof(ptrGBloque) 
#define TAMANIO_CANT_BITMAPS  4
#define TAMANIO_RELLENO       4073  

#define FIN_IDENTIFICADOR    TAMANIO_IDENTIFICADOR
#define FIN_VERSION          FIN_IDENTIFICADOR + TAMANIO_VERSION 
#define FIN_PUNTERO_BITMAP   FIN_VERSION + TAMANIO_PTR_BITMAP
#define FIN_CANTIDAD_BITMAPS TAMANIO_PTR_BITMAP + TAMANIO_CANT_BITMAPS
#define FIN_RELLENO          TAMANIO_CANTIDAD_BITMAPS + TAMANIO_RELLENO

struct t_runtime_options {
	//char* welcome_msg;
} runtime_options;

enum {
	KEY_VERSION,
	KEY_HELP,
};

static int fd;//file descriptor del archivo de grasa
static char* mapped_header; //el array de chars que contiene el archivo via FUSE

static struct grasa_header_t* cargar_header(const char* filename){
	fd = open(filename, O_RDONLY);

	struct stat status;
    fstat (fd, &status);
 	mapped_header = mmap(NULL, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);	

 	if (mapped_header == MAP_FAILED){
 		printf("Error al iniciar mmap con el archivo %s. '%s' \n", filename, strerror(errno));
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
 	}

 	struct grasa_header_t* header = malloc(sizeof(struct grasa_header_t));

	char* identificador = string_substring(mapped_header, 0 , FIN_IDENTIFICADOR);
	strcpy((char*)header->grasa, identificador);
	free(identificador);

	char version_temp[TAMANIO_VERSION];  
	int i;
	for (i = FIN_IDENTIFICADOR; i < FIN_VERSION; ++i)	
		version_temp[i - FIN_IDENTIFICADOR] =  '0' + mapped_header[i];
	
	header->version = strtol(version_temp,NULL,10);
	

	//printf("%i %lu\n" , FIN_VERSION, FIN_PUNTERO_BITMAP );
	char ptrBloque[TAMANIO_PTR_BITMAP];
	for (i = FIN_VERSION; i < FIN_PUNTERO_BITMAP; ++i)
		ptrBloque[i - FIN_VERSION] =  '0' + mapped_header[i];

	header->blk_bitmap = strtol(ptrBloque,NULL,10);
	

	char tamanio_bitmap[TAMANIO_CANT_BITMAPS];
	for (i = TAMANIO_PTR_BITMAP; i < FIN_CANTIDAD_BITMAPS; ++i)	
		tamanio_bitmap[i - TAMANIO_PTR_BITMAP] =  '0' + mapped_header[i];

	header->size_bitmap = strtol(tamanio_bitmap,NULL,10);
	

	char* tamanio_relleno = string_substring(mapped_header, FIN_CANTIDAD_BITMAPS , TAMANIO_RELLENO);;	
	strcpy((char*)header->padding, tamanio_relleno);
	
	free(tamanio_relleno);

 	return header;
}


int main(int argc, char *argv[]){
 	struct grasa_header_t* header = cargar_header("disco.bin"); 
 	

	printf("Identificador\t%s\n", header->grasa);
 	printf("Versión\t%i\n", header->version);
 	printf("Bloque de inicio del bitmap\t%i\n", header->blk_bitmap);	
	printf("Tamaño del Bitmap (en bloques)\t%i\n", header->size_bitmap);
	printf("Relleno lo leo pero no le doy pelota\n");

	return EXIT_SUCCESS;
}



























/*
// Dentro de los argumentos que recibe nuestro programa obligatoriamente
// debe estar el path al directorio donde vamos a montar nuestro FS
int main(int argc, char *argv[]) {
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	return fuse_main(args.argc, args.argv, &fuse_operations, NULL);
}
*/
/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
/*
static struct fuse_opt fuse_options[] = {
		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

static struct fuse_operations fuse_operations = { //las funciones estan en filesystem_operations.c
		.read = fs_read,
		.mkdir = fs_mkdir,
		.create = fs_create,
		.opendir = fs_opendir,
		.write = fs_write,
		.unlink = fs_unlink,
		.rmdir = fs_rmdir,		
};
*/
