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

static int fd;//file descriptor del archivo de grasa
static char* mapped_file = NULL; //el array de chars que contiene el archivo via FUSE
static uint file_size;

static char* iniciar_archivo_mapeado(const char* filename){
	fd = open(filename, O_RDONLY);

	struct stat status;
    fstat (fd, &status);
    file_size = status.st_size;
    
 	mapped_file = mmap(NULL, status.st_size, PROT_READ, MAP_SHARED, fd, 0);	

 	if (mapped_file == MAP_FAILED){
 		printf("Error al iniciar mmap con el archivo %s. '%s' \n", filename, strerror(errno));
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
 	}

 	return mapped_file;
}

static void cerrar_archivo_mapeado(){
	if(mapped_file != NULL)
		munmap(mapped_file, file_size);
}

static char* obtener_nombre_archivo(int argc,char *argv[]){
	if (argc < 2) {
		printf("Especifique el nombre del archivo grasa.\n");
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
	}

	return argv[1]; // 0 es el nombre del proceso.
}

static struct grasa_header_t* cargar_header(const char* filename){
	iniciar_archivo_mapeado(filename);
 	struct grasa_header_t* header = malloc(sizeof(struct grasa_header_t));

	memcpy(header, mapped_file, sizeof(struct grasa_header_t));

 	return header;
}

static t_bitarray* cargar_bitmap(uint32_t cantidad_bloques){
	uint tamanio_total =  cantidad_bloques * TAMANIO_BLOQUE ;
	printf("%i\n",tamanio_total);
	t_bitarray* bitmap = malloc(sizeof(t_bitarray) * tamanio_total);
	//memcpy(&(bitmap), &mapped_file[4096], tamanio_total);
	int i = 0;
	/*for (i = FIN_HEADER; i < FIN_HEADER + tamanio_total; ++i){
		printf("%i",mapped_file[i] );
	}	
	
	int i = 0;

	for (i = FIN_RELLENO; i < FIN_RELLENO + tamanio_total ; i++){
		printf("%i", mapped_file[i]);
		bitmap[i - FIN_RELLENO] = mapped_file[i];
	}
	
	*/
	printf("\n");
	return bitmap;
}

int main(int argc, char *argv[]){
 	struct grasa_header_t* header = cargar_header(obtener_nombre_archivo(argc,argv)); 
 	t_bitarray* grasa_bitmap = cargar_bitmap(header->size_bitmap);

	printf("Identificador:\t%s\n", header->grasa);
 	printf("Versión:\t%i\n", header->version);
 	printf("Bloque de inicio del bitmap:\t%i\n", header->blk_bitmap);	
	printf("Tamaño del Bitmap:\t%i bloque(s)\n", header->size_bitmap);
	printf("Relleno lo leo pero no le doy pelota\n");


	cerrar_archivo_mapeado();
	return EXIT_SUCCESS;
}

