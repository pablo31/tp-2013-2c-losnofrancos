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
#include "../libs/common/string.h"

static int fd;//file descriptor del archivo de grasa
static char* mapped_file = NULL; //el array de chars que contiene el archivo via FUSE
static uint file_size;

static char* iniciar_archivo_mapeado(const char* filename){
	fd = open(filename, O_RDONLY);

	struct stat status;
    fstat (fd, &status);
    file_size = status.st_size;
    //printf("tamaño total del archivo:%i \n", file_size);
 	mapped_file = mmap(NULL, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);	

 	if (mapped_file == MAP_FAILED){
 		printf("Error al iniciar mmap con el archivo %s. '%s' \n", filename, strerror(errno));
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
 	}

 	return mapped_file;
}

static char* obtener_nombre_archivo(int argc,char *argv[]){
	if ( argc < 2) {
		printf("Especifique el nombre del archivo grasa.\n");
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
	}

	return argv[1]; // 0 es el nombre del proceso.
}

static void cargar_identificador(struct grasa_header_t* header ){
	char* identificador = string_substring(mapped_file, 0 , FIN_IDENTIFICADOR);
	strcpy((char*)header->grasa, identificador);
	free(identificador);
}
static void cargar_version(struct grasa_header_t* header){
	char* version_temp = string_new();  
	int i;
	for (i = FIN_IDENTIFICADOR; i < FIN_VERSION; ++i)	
		version_temp[i - FIN_IDENTIFICADOR] =  '0' + mapped_file[i];
	
	header->version = strtol(version_temp,NULL,10);
}
static void cargar_puntero_bloque(struct grasa_header_t* header){
	int i,exponente;
	exponente = TAMANIO_PTR_BITMAP;
	uint valor_puntero = 0;

	for (i = FIN_VERSION; i < FIN_PUNTERO_BITMAP; i++){
		exponente--;
		valor_puntero = valor_puntero + (uint)(mapped_file[i] *  pow(10.0,exponente));
	}

	header->blk_bitmap = valor_puntero;
}
static void cargar_tamanio_bitmap(struct grasa_header_t* header){
	int i,exponente;
	exponente = TAMANIO_CANT_BITMAPS;
	uint valor_bitmap = 0;

	for (i = FIN_PUNTERO_BITMAP; i < FIN_CANTIDAD_BITMAPS; ++i)	{
		exponente--;
		valor_bitmap = valor_bitmap + (uint)(mapped_file[i] *  pow(10.0,exponente));
	}

	header->size_bitmap = valor_bitmap;
}
static void cargar_relleno(struct grasa_header_t* header){
	memcpy(&(header->padding), &mapped_file[FIN_CANTIDAD_BITMAPS], TAMANIO_RELLENO);
}
static struct grasa_header_t* cargar_header(const char* filename){
	iniciar_archivo_mapeado(filename);
 	struct grasa_header_t* header = malloc(sizeof(struct grasa_header_t));

	cargar_identificador(header);
	cargar_version(header);
	cargar_puntero_bloque(header);
	cargar_tamanio_bitmap(header);
	cargar_relleno(header);

 	return header;
}

static bool* cargar_bitmap(uint32_t cantidad_bloques){
	uint tamanio_total =  cantidad_bloques * TAMANIO_BLOQUE ;
	printf("%i\n",tamanio_total);
	bool* bitmap = malloc(sizeof(bool) * tamanio_total);
	//memcpy(&(bitmap), &mapped_file[4096], tamanio_total);
	int i = 0;
	for (i = FIN_HEADER; i < FIN_HEADER + tamanio_total; ++i){
		printf("%i",mapped_file[i] );
	}	
	/*
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
 	bool* grasa_bitmap = cargar_bitmap(header->size_bitmap);

	printf("Identificador:\t%s\n", header->grasa);
 	printf("Versión:\t%i\n", header->version);
 	printf("Bloque de inicio del bitmap:\t%i\n", header->blk_bitmap);	
	printf("Tamaño del Bitmap (en bloques):\t%i\n", header->size_bitmap);
	printf("Relleno lo leo pero no le doy pelota\n");

	return EXIT_SUCCESS;
}

