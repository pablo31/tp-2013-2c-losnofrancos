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
#define FIN_CANTIDAD_BITMAPS FIN_PUNTERO_BITMAP + TAMANIO_CANT_BITMAPS
#define FIN_RELLENO          FIN_CANTIDAD_BITMAPS + TAMANIO_RELLENO


static int fd;//file descriptor del archivo de grasa
static char* mapped_header; //el array de chars que contiene el archivo via FUSE

static char* obtener_nombre_archivo(int argc,char *argv[]){
	if ( argc < 2) {
		printf("Especifique el nombre del archivo grasa.\n");
 		exit(EXIT_FAILURE); //terminamos forzosamente el proceso.
	}

	return argv[1]; // 0 es el nombre del proceso.
}

static void cargar_identificador(struct grasa_header_t* header ){
	char* identificador = string_substring(mapped_header, 0 , FIN_IDENTIFICADOR);
	strcpy((char*)header->grasa, identificador);
	free(identificador);
}
static void cargar_version(struct grasa_header_t* header){
	char* version_temp = string_new();  
	int i;
	for (i = FIN_IDENTIFICADOR; i < FIN_VERSION; ++i)	
		version_temp[i - FIN_IDENTIFICADOR] =  '0' + mapped_header[i];
	
	header->version = strtol(version_temp,NULL,10);
}
static void cargar_puntero_bloque(struct grasa_header_t* header){
	int i,exponente;
	exponente = TAMANIO_PTR_BITMAP;
	uint valor_puntero = 0;

	for (i = FIN_VERSION; i < FIN_PUNTERO_BITMAP; i++){
		exponente--;
		valor_puntero = valor_puntero + (uint)(mapped_header[i] *  pow(10.0,exponente));
	}

	header->blk_bitmap = valor_puntero;
}
static void cargar_bitmap(struct grasa_header_t* header){
	int i,exponente;
	exponente = TAMANIO_CANT_BITMAPS;
	uint valor_bitmap = 0;

	for (i = FIN_PUNTERO_BITMAP; i < FIN_CANTIDAD_BITMAPS; ++i)	{
		exponente--;
		valor_bitmap = valor_bitmap + (uint)(mapped_header[i] *  pow(10.0,exponente));
	}

	header->size_bitmap = valor_bitmap;
}
static void cargar_relleno(struct grasa_header_t* header){
	char* tamanio_relleno = string_substring(mapped_header, FIN_CANTIDAD_BITMAPS , TAMANIO_RELLENO);;	
	strcpy((char*)header->padding, tamanio_relleno);
	
	free(tamanio_relleno);
}
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

	cargar_identificador(header);
	cargar_version(header);
	cargar_puntero_bloque(header);
	cargar_bitmap(header);
	cargar_relleno(header);

 	return header;
}


int main(int argc, char *argv[]){
 	struct grasa_header_t* header = cargar_header(obtener_nombre_archivo(argc,argv)); 
 	

	printf("Identificador:\t%s\n", header->grasa);
 	printf("Versión:\t%i\n", header->version);
 	printf("Bloque de inicio del bitmap:\t%i\n", header->blk_bitmap);	
	printf("Tamaño del Bitmap (en bloques):\t%i\n", header->size_bitmap);
	printf("Relleno lo leo pero no le doy pelota\n");

	return EXIT_SUCCESS;
}

