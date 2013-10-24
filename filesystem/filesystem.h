#define TAMANIO_IDENTIFICADOR 5
#define TAMANIO_VERSION       4
#define TAMANIO_PTR_BITMAP    sizeof(ptrGBloque) 
#define TAMANIO_CANT_BITMAPS  4
#define TAMANIO_RELLENO       4073  
#define TAMANIO_BLOQUE 		  4096

#define FIN_IDENTIFICADOR    TAMANIO_IDENTIFICADOR
#define FIN_VERSION          FIN_IDENTIFICADOR + TAMANIO_VERSION 
#define FIN_PUNTERO_BITMAP   FIN_VERSION + TAMANIO_PTR_BITMAP
#define FIN_CANTIDAD_BITMAPS FIN_PUNTERO_BITMAP + TAMANIO_CANT_BITMAPS
#define FIN_RELLENO          FIN_CANTIDAD_BITMAPS + TAMANIO_RELLENO
#define FIN_HEADER			 TAMANIO_BLOQUE