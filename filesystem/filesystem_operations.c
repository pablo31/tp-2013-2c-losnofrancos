#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include "../libs/logger/logger.h"
#include "../libs/common/bitarray.h"
#include "../libs/common/string.h"
//#include <fcntl.h>
#include "grasa.h"
#include "filesystem_operations.h"

extern tad_logger* logger;
extern char* mmaped_file;
extern struct grasa_header_t* header;
extern t_bitarray* grasa_bitmap;
extern GFile* nodos;
void cargar_datos(GFile archivo, char* buffer, size_t size, off_t offset);


void logear_path(const char* funcion, const char* path) {
	logger_info(logger, "\tFuncion:'%s'", funcion);
	//logger_info(logger, "");
	logger_info(logger, "\tpath:'%s'", path);
	logger_info(logger, "-----------------");
}

/*
 Read data from an open file

 Read should return exactly the number of bytes requested except on EOF or error, otherwise the rest of the data will be substituted
 with zeroes. An exception to this is when the 'direct_io' mount option is specified, in which case the return value of the read system
 call will reflect the return value of this operation.
 */
int fs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {

	//logear_path("fs_read", path);
	logger_info(logger,"fs_read: '%s' %zu bytes desde byte %i.", path, size,offset);

	uint indice = 0;
	int retorno = buscar_bloque_nodo(path, &indice);

	if (retorno){ // no existe.
		logger_info(logger,"fs_read: no encontre '%s' indice: '%i'.", path, indice);
		return -ENOENT;
	}

	GFile nodo = nodos[indice];
	cargar_datos(nodo, buf, size, offset);

	return EXIT_SUCCESS;
}

/*
 Create a directory

 Note that the mode argument may not have the type specification bits set, i.e. S_ISDIR(mode) can be false. To obtain the correct
 directory type bits use mode|S_IFDIR
 */
int fs_mkdir(const char *path, mode_t mode) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	char** subpath;
	char* directorio = NULL;
	int i = 0;
	int err = 0;
	GFile nodo;
	uint32_t bloque_padre = 0;

	//logger_info(logger, "Creo directorio:");
	//logear_path("fs_mkdir", path);
	if (strcmp(path, "/") == 0) {
			return -EPERM;
		}
	subpath = string_split(temp, "/");
	while (subpath[i] != NULL ) {

		bloque_padre = nodo.parent_dir_block;
		directorio = subpath[i];
		err = buscar_bloque_por_padre(directorio, bloque_padre,
				&nodo.parent_dir_block);
		if (err && subpath[i + 1] != NULL ) {
			return EXIT_FAILURE;
		};

		i++;
	};

	//nodo.c_date = time(NULL );
	//nodo.m_date = nodo.c_date;
	nodo.file_size = 0;
	strcpy(nodo.fname, directorio);
	nodo.state = 2; // 0: borrado, 1: archivo, 2: directorio

	return agregar_nodo(nodo);
}

/*
 Create and open a file

 If the file does not exist, first create it with the specified mode, and then open it.

 If this method is not implemented or under Linux kernel versions earlier than 2.6.15, the mknod() and open() methods will be called instead.
 */
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	//logger_info(logger, "Creo archivo:");
	//logear_path("fs_create", path);
	;
	return 0;
}

/*
 Open directory

 Unless the 'default_permissions' mount option is given, this method should check if opendir is permitted for this directory. Optionally opendir may also return an arbitrary filehandle in the fuse_file_info structure, which will be passed to readdir, closedir and fsyncdir.

 */
int fs_opendir(const char *path, struct fuse_file_info *fi) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	//logear_path("fs_opendir", path);
	int err = 0;
	uint32_t bloque;
	if (strcmp(path, "/") == 0) {
		return EXIT_SUCCESS;
	}

	err = buscar_bloque_nodo(temp, &bloque);

	if (!err && nodos[bloque].state == 2) {
		return EXIT_SUCCESS;
	} else {
		return -ENOENT;
	}

}
/*
 Write data to an open file

 Write should return exactly the number of bytes requested except on error. An exception to this is when the 'direct_io' mount option is specified (see read operation).
 */
int fs_write(const char *path, const char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	//logger_info(logger, "Escribo archivo:");
	//logear_path("fs_write", path);
	return 0;
}

/*
 Remove a file
 */
int fs_unlink(const char *path) {
	//logger_info(logger, "Elimino archivo:");
	//logear_path("fs_unlink", path);

	return 0;
}

/*
 Remove a directory
 */
int fs_rmdir(const char *path) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	int err = 0;
	uint32_t bloque = 0;

	//logger_info(logger, "Elimino directorio");
	//logear_path("fs_rmdir", temp);
	if (strcmp(path, "/") == 0) {
		return -EPERM;
	}

	err = buscar_bloque_nodo(temp, &bloque);
	if (err) {
		return -ENOENT;
	}
	logger_info(logger, "bloque:%i", bloque);
	if(nodos[bloque].state != 2){
		return -ENOTDIR;
	}

	return borrar_nodo(bloque);

}

int fs_getattr(const char * path, struct stat *stat) {
	//logger_info(logger, "get attributes");
	//logear_path("fs_getattr", path);
	char* temp = string_from_format(path, "%s");
	uint32_t bloque = 0;
	int rc = 0; //Return code
	memset(stat, 0, sizeof(struct stat));

	if (strcmp(path, "/") == 0) {
		stat->st_mode = S_IFDIR | 0755;
		stat->st_nlink = 2;
		//logger_info(logger, "default");
	} else {
		rc = buscar_bloque_nodo(temp, &bloque);
		if (!rc) {
			stat->st_nlink = nodos[bloque].state;
			if (nodos[bloque].state == 1) {
				stat->st_mode = S_IFREG | 0444;
				stat->st_size = nodos[bloque].file_size;
				stat->st_nlink = 1;
			} else {
				stat->st_mode = S_IFDIR | 0755;
				stat->st_nlink = 2;
			}
			//stat->st_uid = 1000;
			//stat->st_gid = 1000;
			stat->st_atim.tv_sec = nodos[bloque].m_date;
			stat->st_mtim.tv_sec = nodos[bloque].m_date;
			stat->st_ctim.tv_sec = nodos[bloque].c_date;
		} else {
			return -ENOENT;
		}
	}
	/*
	 stat->st_uid = 1000;
	 stat->st_gid = 1000;
	 */
	logger_info(logger, "nlink =%i, Mode = %i", stat->st_nlink, stat->st_mode);

	return EXIT_SUCCESS;
}

int fs_open(const char *path, struct fuse_file_info *fi) {
	//logger_info(logger, "Abrir");
	//logear_path("fs_open", path);
	logear_path("fs_open", path);

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

int fs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	uint32_t bloque_padre;
	int rc = 0; //return code

	//logger_info(logger, "Leer directorio");
	//logear_path("fs_readdir", path);

	//Agrego las opciones . y .. por defecto
	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);

	// Busco todo el contenido del directorio
	if(strcmp(path, "/") == 0){
		bloque_padre = 0;
	}
	else{
	rc = buscar_bloque_nodo(temp, &bloque_padre);
	}
	if (!rc) {
		rc = buscar_nodos_por_padre(bloque_padre, buffer, filler);
		if (rc) {
			return -ENOENT;
		}
	} else {
		return -ENOENT;
	}

	return EXIT_SUCCESS;
}

//-------------------------------------------------------------------
// Funciones auxiliares:											|
//-------------------------------------------------------------------
int agregar_nodo(const GFile nodo) {
	bool fin = false;
	int i = 1;

	while (!fin && i < GFILEBYTABLE + 1) {
		if (nodos[i].state == 0) {
			nodos[i] = nodo;
			fin = true;
		}

		i++;
	}

	if (fin)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;

}

/*
 * Cambio el estado de un nodo a borrado.
 */
int borrar_nodo(const uint32_t bloque) {

	if (nodos[bloque].state != 0) {
		nodos[bloque].state = 0;
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}

}

bool es_nodo_por_nombre(GFile nodo, char* nombre) {
	return !strncmp((char*) nodo.fname, nombre, GFILENAMELENGTH);
}

bool es_nodo_por_padre(GFile nodo, uint32_t padre) {
	return nodo.parent_dir_block == padre;
}

/* Busco el nodo con nombre "fname" y cuyo padre es el bloque "bloque_padre"
 * y lo devuelvo en "bloque"
 */
int buscar_bloque_por_padre(char *fname, uint32_t bloque_padre,
		uint32_t *bloque) {
	bool encontrado = false;
	uint32_t i = 1;
	GFile nodo;

	while (!encontrado && i < (GFILEBYTABLE + 1)) {
		nodo = nodos[i];

		if (es_nodo_por_nombre(nodo, fname)
				&& es_nodo_por_padre(nodo, bloque_padre) && nodo.state != 0) {
			*bloque = i;
			encontrado = true;
		}
		i++;
	}

	if (encontrado)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

// con el path completo busco el bloque en donde esta guardado ese nodo y lo devuelvo
int buscar_bloque_nodo(char* path, uint32_t *bloque) {
	char** subpath;
	int i = 0;
	int err = 0;
	uint32_t bloque_padre = 0;
	*bloque = 0;

	subpath = string_split(path, "/");
	while (subpath[i] != NULL ) {
		//logger_info(logger, "substring:%s",subpath[i] );
		bloque_padre = *bloque;
		err = buscar_bloque_por_padre(subpath[i], bloque_padre, bloque);
		//logger_info(logger, "bloque:%i, Padre:%i",*bloque,bloque_padre );

		if (err) {
			free(subpath);
			return EXIT_FAILURE;
		}

		free(subpath[i]);
		i++;

	};

	free(subpath);
	return EXIT_SUCCESS;
}

// Busca todos los nodos que tienen como padre al bloque "bloque_padre"
int buscar_nodos_por_padre(uint32_t bloque_padre, void *buffer,
		fuse_fill_dir_t filler) {
	//t_list *contenido) {
	int i;
	//char * nombre;
	for (i = 1; i < (GFILEBYTABLE + 1); i++) {
		if (nodos[i].parent_dir_block == bloque_padre && nodos[i].state != 0) {
			//nombre = malloc(sizeof(nodos[i].fname));
			//memcpy(nombre, nodos[i].fname, GFILENAMELENGTH - 1);
			//nombre[GFILENAMELENGTH - 1] = '\0';
			//list_add(contenido, nombre);
			filler(buffer, nodos[i].fname, NULL, 0);
		}
	}
	return EXIT_SUCCESS;
}

bool existe_puntero(ptrGBloque ptr) {
	return ptr != 0;
}

bool estoy_en_rango(uint base, uint offset, uint valor){
	return (valor >= base) && (valor <= (base+offset));
}

void* buscar_nodo(uint indice) {
	return mmaped_file + (indice * 4096);
}

/*
 * Cargo los datos del archivo de grasa en el buffer , con offset e inicio especificados.	
 */

void cargar_datos(GFile archivo, char* buffer, size_t offset, off_t inicio){	
	uint i = 0;// Indice de los punteros a bloques de punteros. Max 1000.
	uint j = 0;// Indice de los punteros a bloques de datos. Max 1024
	uint k = 0;// Hasta donde cargue en el buffer. Max valor del offset.
	bool fin = false;
	char* nodo_datos = NULL; //Nodo con datos a cargar en el buffer.
	ptrGBloque* punteros_a_datos = NULL;
	
	while (!fin && i < 1000 && existe_puntero(archivo.blk_indirect[i]) ){	
		//cargo los punteros de los nodos de datos en mi lista de punteros	
		punteros_a_datos = (ptrGBloque*) buscar_nodo(archivo.blk_indirect[i]);

		j = 0;
		while(!fin && j < 1024 && existe_puntero(punteros_a_datos[j])){
			nodo_datos = buscar_nodo(punteros_a_datos[j]);

			if (estoy_en_rango(inicio, offset, j)) {
				uint cuantos_bytes_cargar = 0;

				if ((offset - k) < 4096)					
					cuantos_bytes_cargar = (offset - k); //Lo que me falta para el offset.
				else					
					cuantos_bytes_cargar = 4096;//cargo todo el bloque.

				logger_info(logger,"fs_read: k:%u offset:%u falta:%u",k , offset, offset - k);	
				memcpy(buffer + k, nodo_datos, cuantos_bytes_cargar);
				k = k + cuantos_bytes_cargar;			
			}

			j++;
			fin = k >= (inicio+offset); //estamos leyendo datos mas alla de lo pedido?

			if (fin)	
				logger_info(logger,"fs_read: fin lectura");
		}
		
		i++;
	}
	//logger_info(logger,"fs_read: buffer: %s",buffer);
	//logger_info(logger,"fs_read: strlen(buffer):%u ", strlen(buffer) );
	//free(nodo_datos);
	logger_info(logger,"fs_read: buffer: %s",buffer);
}

