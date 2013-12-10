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
extern t_bitarray* bitmap;
extern GFile* nodos;
extern sem_t mutex_nodos, mutex_mapa, mutex_datos;
extern uint bitmap_bytes_usados;

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
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	//logear_path("fs_read", path);
	//logger_info(logger, "fs_read: '%s' %zu bytes desde byte %i.", path, size,offset);

	uint indice = 0;
	int retorno = buscar_bloque_nodo(temp, &indice);

	free(temp);

	if (retorno) { // no existe.
		/*logger_info(logger, "fs_read: no encontre '%s' indice: '%i'.", temp,
		 indice);*/
		return -ENOENT;
	}
	return cargar_datos(&(nodos[indice]), buf, size, offset);
}

/*
 Create a directory

 Note that the mode argument may not have the type specification bits set, i.e. S_ISDIR(mode) can be false. To obtain the correct
 directory type bits use mode|S_IFDIR
 */
int fs_mkdir(const char *path, mode_t mode) {
	char* temp; // = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	char** subpath;
	char* directorio = NULL;
	int i = 0;
	int err = 0;
	int ret = 0;
	GFile nodo;
	uint32_t bloque_padre = 0;

	//logger_info(logger, "Creo directorio:");
	//logear_path("fs_mkdir", path);
	if (strcmp(path, "/") == 0) {
		return -EPERM;
	}
	temp = string_from_format(path, "%s");
	subpath = string_split(temp, "/");
	free(temp);
	while (subpath[i] != NULL ) {
		if (i == 2)
			ret = -EPERM;

		bloque_padre = nodo.parent_dir_block;
		directorio = subpath[i];
		err = buscar_bloque_por_padre(directorio, bloque_padre,
				&nodo.parent_dir_block);
		if (err && subpath[i + 1] != NULL )
			ret = -ENOENT;

		i++;
	};
	strcpy((char *) nodo.fname, directorio);
	while (subpath[i] != NULL ) {
		free(subpath[i]);
	}
	free(subpath);
	if (ret)
		return ret;
	if (err == 0) { //Si la ultima busqueda no fallo significa que ya existe
		return -EEXIST;
	}
	nodo.c_date = time(NULL );
	nodo.m_date = nodo.c_date;
	nodo.file_size = 0;
	nodo.state = 2; // 0: borrado, 1: archivo, 2: directorio

	return agregar_nodo(nodo);
}

/*
 Create and open a file

 If the file does not exist, first create it with the specified mode, and then open it.

 If this method is not implemented or under Linux kernel versions earlier than 2.6.15, the mknod() and open() methods will be called instead.
 */
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	char* temp; // = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	char** subpath;
	char* directorio = NULL;
	int i = 0;
	int err = 0;
	int ret = 0;
	GFile nodo;
	uint32_t bloque_padre = 0;

	//logger_info(logger, "Creo archivo:");
	//logear_path("fs_create", path);

	if (strcmp(path, "/") == 0) {
		return -EPERM;
	}
	temp = string_from_format(path, "%s");
	subpath = string_split(temp, "/");
	free(temp);
	while (subpath[i] != NULL ) {
		bloque_padre = nodo.parent_dir_block;
		directorio = subpath[i];
		err = buscar_bloque_por_padre((char *) directorio, bloque_padre,
				&nodo.parent_dir_block);
		if (err && subpath[i + 1] != NULL ) {
			ret = -ENOENT;
		};
		i++;
	};
	strcpy((char *) nodo.fname, directorio);
	liberar_subpath(subpath);
	if (ret)
		return ret;
	if (err == 0) { //Si la ultima busqueda no fallo significa que ya existe
		return -EEXIST;
	}
	nodo.c_date = time(NULL );
	nodo.m_date = nodo.c_date;
	nodo.file_size = 0;
	nodo.state = 1; // 0: borrado, 1: archivo, 2: directorio
	memset(nodo.blk_indirect, 0, sizeof(ptrGBloque) * 1000);

	return agregar_nodo(nodo);

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
		free(temp);
		return EXIT_SUCCESS;
	}

	err = buscar_bloque_nodo(temp, &bloque);
	free(temp);
	sem_wait(&mutex_nodos);
	if (!err && nodos[bloque].state == 2) {
		sem_post(&mutex_nodos);
		return EXIT_SUCCESS;
	} else {
		sem_post(&mutex_nodos);
		return -ENOENT;
	}

}
/*
 Write data to an open file

 Write should return exactly the number of bytes requested except on error. An exception to this is when the 'direct_io' mount option is specified (see read operation).
 */
int fs_write(const char *path, const char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	//logger_info(logger, "Escribo archivo:");
	//logear_path("fs_write", path);
	int ret = 0;

	uint indice = 0;
	int err = buscar_bloque_nodo(temp, &indice);
	free(temp);

	if (err) { // no existe.}
		return 0;
	}
	/*sem_wait(&mutex_nodos);
	 GFile nodo = nodos[indice];
	 sem_post(&mutex_nodos);*/
	ret = guardar_datos(&(nodos[indice]), buf, size, offset);
	if (!ret) {
		return size;
	} else {
		return 0;
	}

}

/*
 Remove a file
 */
int fs_unlink(const char *path) {
	//logger_info(logger, "Elimino archivo:");
	//logear_path("fs_unlink", path);
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	int err = 0;
	uint32_t bloque = 0;

	err = buscar_bloque_nodo(temp, &bloque);
	free(temp);
	if (err) {
		return -ENOENT;
	}
	err = borrar_nodo(bloque);
	if (err) {
		return err;
	}
	// marcar los bloques en el mapa de bits como vacios
	err = liberar_espacio(bloque);
	if (err) {
		return err;
	}
	return EXIT_SUCCESS;
}

/*
 Remove a directory
 */
int fs_rmdir(const char *path) {
	char* temp = string_from_format(path, "%s"); // no uso string_duplicate para evitar el warning de tipos.
	int err = 0;
	int hijos = 0;
	uint32_t bloque = 0;

	int filler(void *buf, const char *name, const struct stat *stbuf, off_t off) {
		int *i;
		i = buf;
		*i = *name;
		return 0;
	}
	//logger_info(logger, "Elimino directorio");
	//logear_path("fs_rmdir", temp);
	if (strcmp(path, "/") == 0) {
		return -EPERM;
	}

	err = buscar_bloque_nodo(temp, &bloque);
	free(temp);
	if (err) {
		return -ENOENT;
	}
	err = buscar_nodos_por_padre(bloque, (void *) &hijos, filler);
	if (err || hijos)
		return -EPERM;

	sem_wait(&mutex_nodos);
	if (nodos[bloque].state != 2) {
		sem_post(&mutex_nodos);
		return -ENOTDIR;
	}
	sem_post(&mutex_nodos);
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
			sem_wait(&mutex_nodos);
			stat->st_nlink = nodos[bloque].state;
			if (nodos[bloque].state == 1) {
				stat->st_mode = S_IFREG | 0777;
				stat->st_size = nodos[bloque].file_size;
				stat->st_nlink = 1;
			} else {
				stat->st_mode = S_IFDIR | 0777;
				stat->st_nlink = 2;
			}
			//stat->st_uid = 1000;
			//stat->st_gid = 1000;
			stat->st_atim.tv_sec = nodos[bloque].m_date;
			stat->st_mtim.tv_sec = nodos[bloque].m_date;
			stat->st_ctim.tv_sec = nodos[bloque].c_date;
			sem_post(&mutex_nodos);
		} else {
			free(temp);
			return -ENOENT;
		}
	}
	/*
	 stat->st_uid = 1000;
	 stat->st_gid = 1000;
	 */
	//logger_info(logger, "nlink =%i, Mode = %i", stat->st_nlink, stat->st_mode);
	free(temp);
	return EXIT_SUCCESS;
}

int fs_open(const char *path, struct fuse_file_info *fi) {
	int err = 0;
	char* temp;
	uint32_t bloque = 0;
	//logger_info(logger, "Abrir");
	//logear_path("fs_open", path);
	temp = string_from_format(path, "%s");
	err = buscar_bloque_nodo(temp, &bloque);
	//logger_info(logger, "fs_open: encontre '%s' ? '%s'.", path,
	//(err) ? "no" : "si");
	if (err) {
		free(temp);
		return -ENOENT;
	}

	free(temp);
	return EXIT_SUCCESS;
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

	// Busco el contenido del directorio completo
	if (strcmp(path, "/") == 0) {
		bloque_padre = 0;
	} else {
		rc = buscar_bloque_nodo(temp, &bloque_padre);
	}
	if (!rc) {
		rc = buscar_nodos_por_padre(bloque_padre, buffer, filler);
		if (rc) {
			free(temp);
			return -ENOENT;
		}
	} else {
		free(temp);
		return -ENOENT;
	}

	free(temp);
	return EXIT_SUCCESS;
}

int fs_truncate(const char * path, off_t size) {
	logger_info(logger, "path:%s /noffset:%i", path, size);

	char* temp = string_from_format(path, "%s");
	uint32_t bloque = 0;
	int rc = 0; //Return code

	rc = buscar_bloque_nodo(temp, &bloque);
	free(temp);
	if (!rc) {
		sem_wait(&mutex_nodos);
		nodos[bloque].file_size = size;
		sem_post(&mutex_nodos);
	} else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/*
 * Funcion dummy
 */
int fs_utime(const char * path, struct utimbuf * utimbuf) {

	return 0;
}
//-------------------------------------------------------------------
// Funciones auxiliares:											|
//-------------------------------------------------------------------
int agregar_nodo(const GFile nodo) {
	bool fin = false;
	int i = 1;

	while (!fin && i < GFILEBYTABLE + 1) {
		sem_wait(&mutex_nodos);
		if (nodos[i].state == 0) {
			nodos[i] = nodo;
			fin = true;
		}
		sem_post(&mutex_nodos);
		i++;
	}

	if (fin)
		return EXIT_SUCCESS;
	else
		return -ENOSPC;

}

/*
 * Cambio el estado de un nodo a borrado.
 */
int borrar_nodo(const uint32_t bloque) {
	int ret;
	sem_wait(&mutex_nodos);
	if (nodos[bloque].state != 0) {
		nodos[bloque].state = 0;
		ret = EXIT_SUCCESS;
	} else {
		ret = -EAGAIN;
	}
	sem_post(&mutex_nodos);
	return ret;
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
		sem_wait(&mutex_nodos);
		nodo = nodos[i];
		sem_post(&mutex_nodos);
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

/*
 * Libero todos los strings de un subpath.
 *
 * Braulio, hago este metodo porque estaba perdiendo memoria buscar_bloque_nodo.
 * Si el while encontraba un error y retornaba , no liberaba los siguientes nodos del path.
 */
void liberar_subpath(char** subpath) {
	int i = 0;
	while (subpath[i] != NULL ) {
		free(subpath[i]);
		i++;
	};

	free(subpath);
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
			liberar_subpath(subpath);
			//free(subpath);
			return EXIT_FAILURE;
		}

		//free(subpath[i]);
		i++;

	};

	liberar_subpath(subpath);
	return EXIT_SUCCESS;
}

// Busca todos los nodos que tienen como padre al bloque "bloque_padre"
int buscar_nodos_por_padre(uint32_t bloque_padre, void *buffer,
		fuse_fill_dir_t filler) {
	GFile nodo;
	int i;

	for (i = 1; i < (GFILEBYTABLE + 1); i++) {
		sem_wait(&mutex_nodos);
		nodo = nodos[i];
		sem_post(&mutex_nodos);
		if (nodo.parent_dir_block == bloque_padre && nodo.state != 0) {
			filler(buffer, (char*) nodo.fname, NULL, 0);
		}
	}
	return EXIT_SUCCESS;
}

bool no_existe_puntero(ptrGBloque ptr) {
	return ptr == 0;
}

bool estoy_en_rango(uint base, uint offset, uint valor) {
	return (valor >= base) && (valor <= (base + offset));
}

void* buscar_nodo(uint indice) {
	return mmaped_file + (indice * 4096);
}

/*
 * Dado un indice de bytes de un archivo grasa , devuelvo en que indice de nodos de punteros esta. 
 * Cada nodo de punteros tiene 1024 punteros a nodos de datos
 */
uint get_nodo_punteros_inicial(off_t inicio) {
	//logger_info(logger, "inicio:%d", inicio);
	uint temp = inicio / (1024 * 4096);
	//logger_info(logger, "_indirectos:%d", temp);
	return temp;
}
/*
 * Dado un indice de bytes de un archivo grasa y un indice de nodos de punteros es, 
 * devuelvo en que indice de bloques de datos empiezan los datos 
 */
uint get_nodo_datos_inicial(off_t inicio, uint indice_nodos_punteros) {
	//logger_info(logger, "inicio:%d", inicio);
	uint temp = inicio - (indice_nodos_punteros * 1024 * 4096);
	temp = temp / 4096;
	//logger_info(logger, "_directos:%d", temp);
	return temp;
}

/*
 * Dado un indice de archivo calculo el offset desde el cual deberia empezar
 * dentro del bloque de datos
 */
uint get_offset_bloque(off_t inicio, uint indice_nodos_punteros) {
	//logger_info(logger, "inicio:%d", inicio);
	uint temp = inicio - (indice_nodos_punteros * 1024 * 4096);
	temp = temp % 4096;
	//logger_info(logger, "bloque:%i", temp);
	return temp;
}

/*
 * Un archivo grasa esta definido como hasta 1000 punteros a bloques 
 * y cada bloque tiene 1024 punteros a bloques de datos.
 */
uint get_offset_bytes_archivo(uint indice_nodos_punteros,
		uint indice_nodos_datos) {
	uint retorno = 4096
			* (1000 * 1024 * indice_nodos_punteros + indice_nodos_datos);
	//logger_info(logger,"fs_read: offset_bytes_archivo:%u",retorno);
	return retorno;
}
/*
 * Devuelve el indice un nodo vacio segun el campo bitmap
 */
uint get_indice_nodo_vacio() {
	/*  1 bloque header
	 n bloques del bitmap
	 1024 bloques de nodos
	 1 nodo = 4096 bytes */
	uint inicio_datos = (header->size_bitmap + 1024 + 1);	// * TAMANIO_BLOQUE;
	bool fin = false;

	while (!fin && (bitmap_bytes_usados * 8) > inicio_datos) {
		inicio_datos++;
		sem_wait(&mutex_mapa);
		fin = !bitarray_test_bit(bitmap, inicio_datos);
		if (fin) {
			bitarray_set_bit(bitmap, inicio_datos);
		}
		sem_post(&mutex_mapa);
	}
	if (fin)
		return inicio_datos;
	else
		return 0;
}

void set_bloque_usado(uint indice_bloque) {
	sem_wait(&mutex_mapa);
	bitarray_set_bit(bitmap, indice_bloque);
	sem_post(&mutex_mapa);
}

void set_bloque_libre(uint indice_bloque) {
	sem_wait(&mutex_mapa);
	bitarray_clean_bit(bitmap, indice_bloque);
	sem_post(&mutex_mapa);
}

int guardar_datos(GFile* archivo, const char* buffer, size_t size, off_t offset) {
	if (archivo == NULL )
		return EXIT_FAILURE;

	bool inicializar = false;
	uint indice_indirectos, indice_directos, offset_bloque, bytes_guardados,
			cuantos_bytes_guardar;
	bool fin = false;
	char* nodo_datos = NULL; //Nodo con datos a cargar en el buffer.
	ptrGBloque* nodo_punteros = NULL;

	indice_indirectos = get_nodo_punteros_inicial(offset);// Indice de los punteros a bloques de punteros. Max 1000.
	indice_directos = get_nodo_datos_inicial(offset, indice_indirectos);// Indice de los punteros a bloques de datos. Max 1024.
	offset_bloque = get_offset_bloque(offset, indice_indirectos);
	bytes_guardados = 0;

	//logger_info(logger, "fs_write: size:%u offset:%u", size, offset);
	//logger_info(logger, "indirectos:%u, directos: %u, bloque: %u", indice_indirectos, indice_directos, offset_bloque);

	while (!fin && indice_indirectos < 1000) {
		sem_wait(&mutex_nodos);
		if (no_existe_puntero(archivo->blk_indirect[indice_indirectos])) {
			archivo->blk_indirect[indice_indirectos] = get_indice_nodo_vacio();
			inicializar = true;
		}
		sem_post(&mutex_nodos);

		if (no_existe_puntero(archivo->blk_indirect[indice_indirectos]))
			return -ENOSPC;

		nodo_punteros = buscar_nodo(archivo->blk_indirect[indice_indirectos]);
		if (inicializar) {
			inicializar = false;
			sem_wait(&mutex_datos);
			memset(nodo_punteros, 0, TAMANIO_BLOQUE);
			sem_post(&mutex_datos);
		}

		//logger_info(logger, "levanto bloque %i", archivo->blk_indirect[indice_indirectos]);
		while (!fin && indice_directos < 1024) { //&& existe_puntero(punteros_a_datos[j])){
			sem_wait(&mutex_datos);
			if (no_existe_puntero(nodo_punteros[indice_directos])) {
				nodo_punteros[indice_directos] = get_indice_nodo_vacio();
				inicializar = true;
			}
			sem_post(&mutex_datos);

			if (no_existe_puntero(nodo_punteros[indice_directos]))
				return -ENOSPC;

			nodo_datos = buscar_nodo(nodo_punteros[indice_directos]);

			if (inicializar) {
				inicializar = false;
				sem_wait(&mutex_datos);
				memset(nodo_datos, 0, TAMANIO_BLOQUE);
				sem_post(&mutex_datos);
			}

			//offset_bytes_archivo = get_offset_bytes_archivo(indice_indirectos, indice_directos);

			cuantos_bytes_guardar = 0;

			//logger_info(logger, "bytes_guardados:%i", bytes_guardados);
			if ((size - bytes_guardados) < (4096 - offset_bloque)) {
				cuantos_bytes_guardar = size - bytes_guardados; //Lo que me falta para el offset.
				//logger_info(logger, "lo que falta");
			} else {
				cuantos_bytes_guardar = 4096 - offset_bloque; //cargo el bloque completo.
				//logger_info(logger, "bloque completo");
			}
			//logger_info(logger,"se guardan:%u",cuantos_bytes_guardar);
			sem_wait(&mutex_datos);
			memcpy(nodo_datos + offset_bloque, buffer + bytes_guardados,
					cuantos_bytes_guardar);
			sem_post(&mutex_datos);
			bytes_guardados = bytes_guardados + cuantos_bytes_guardar;
			offset_bloque = 0;

			indice_directos++;
			fin = bytes_guardados >= size; //estamos leyendo datos mas alla de lo pedido?
			//logger_info(logger, "fin:%i, k: %i,", fin, bytes_guardados);
		}
		indice_directos = 0;
		indice_indirectos++;
	}

	/*
	 uint i = get_nodo_punteros_inicial(inicio);// Indice de los punteros a bloques de punteros. Max 1000.
	 uint j = get_nodo_datos_inicial(i,inicio);// Indice de los punteros a bloques de datos. Max 1024
	 bool fin = false;
	 */
	sem_wait(&mutex_nodos);
	if (archivo->file_size < (size + offset)) {
		archivo->file_size = (size + offset);
	}
	sem_post(&mutex_nodos);
	if (!fin) {
		return -EFBIG;
	}

	return EXIT_SUCCESS;
}

/*
 * Cargo los datos del archivo de grasa en el buffer , con offset e inicio especificados.	
 */
uint cargar_datos(GFile *archivo, char* buffer, size_t size, off_t inicio) {
	uint indice_indirectos = get_nodo_punteros_inicial(inicio);	// Indice de los punteros a bloques de punteros. Max 1000.
	uint indice_directos = get_nodo_datos_inicial(inicio, indice_indirectos);// Indice de los punteros a bloques de datos. Max 1024.
	uint offset_bloque = get_offset_bloque(inicio, indice_indirectos);
	uint bytes_leidos = 0;// Hasta donde guarde en el buffer. Max valor del offset.
	uint offset_bytes_archivo; // Offset de los bytes del archivo.
	bool fin = false;
	char* nodo_datos = NULL; //Nodo con datos a cargar en el buffer.
	ptrGBloque* punteros_a_datos;

	sem_wait(&mutex_nodos);
	if ((inicio + size) > archivo->file_size)
		size = archivo->file_size - inicio;
	sem_post(&mutex_nodos);
	//logger_info(logger, "fs_write: size:%u offset:%u", size, inicio);
	//logger_info(logger, "indirectos:%u, directos: %u, bloque: %u", indice_indirectos, indice_directos, offset_bloque);
	sem_wait(&mutex_nodos);
	while (!fin && indice_indirectos < 1000
			&& !no_existe_puntero(archivo->blk_indirect[indice_indirectos])) {

		punteros_a_datos = buscar_nodo(archivo->blk_indirect[indice_indirectos]);
		sem_post(&mutex_nodos);
		sem_wait(&mutex_datos);
		while (!fin && indice_directos < 1024
				&& !no_existe_puntero(punteros_a_datos[indice_directos])) {
			nodo_datos = buscar_nodo(punteros_a_datos[indice_directos]);
			sem_post(&mutex_datos);

			offset_bytes_archivo = get_offset_bytes_archivo(indice_indirectos,
					indice_directos);

			//logger_info(logger,"fs_read: estoy_en_rango:%s inicio:%u offset:%u j:%u",
			//	estoy_en_rango(inicio, offset, offset_bytes_archivo + j) ? "si":"no" , inicio,offset,offset_bytes_archivo + j);

			if (estoy_en_rango(inicio, size,
					offset_bytes_archivo + indice_directos)) {
				uint cuantos_bytes_cargar = 0;

				if ((size - bytes_leidos) < (4096 - offset_bloque))
					cuantos_bytes_cargar = size - bytes_leidos; //Lo que falta
				else
					cuantos_bytes_cargar = 4096 - offset_bloque; //cargo el bloque completo.

				//logger_info(logger,"se cargan:%u",cuantos_bytes_cargar);
				if (cuantos_bytes_cargar != 0) {
					//logger_info(logger,"fs_read: k:%u offset:%u falta:%u",k , offset, offset - k);
					sem_wait(&mutex_datos);
					memcpy(buffer + bytes_leidos, nodo_datos + offset_bloque,
							cuantos_bytes_cargar);
					sem_post(&mutex_datos);
					bytes_leidos = bytes_leidos + cuantos_bytes_cargar;
					offset_bloque = 0;
				}
			}

			indice_directos++;
			sem_wait(&mutex_nodos);
			fin = bytes_leidos >= (inicio + size)
					|| (bytes_leidos + inicio) >= archivo->file_size; //estamos leyendo datos mas alla de lo pedido?
			sem_post(&mutex_nodos);
					//if (fin)
					//logger_info(logger,"fs_read: fin lectura");
			sem_wait(&mutex_datos);
		}
		sem_post(&mutex_datos);
		indice_directos = 0;
		indice_indirectos++;
		sem_wait(&mutex_nodos);
	}
	sem_post(&mutex_nodos);
	//logger_info(logger,"fs_read: buffer: %s",buffer);
	return bytes_leidos;
}

int liberar_espacio(uint32_t bloque) {
	uint indice_indirectos = 0;	// Indice de los punteros a bloques de punteros. Max 1000.
	uint indice_directos = 0;// Indice de los punteros a bloques de datos. Max 1024
	bool usado;
	//ptrGBloque* punteros_a_datos = NULL;
	ptrGBloque punteros_a_datos[GFILEBYTABLE];
	GFile nodo;

	sem_wait(&mutex_nodos);
	nodo = nodos[bloque];
	sem_post(&mutex_nodos);

	while (indice_indirectos < 1000
			&& !no_existe_puntero(nodo.blk_indirect[indice_indirectos])) {
		//cargo los punteros de los nodos de datos en mi lista de punteros
		//punteros_a_datos = (ptrGBloque*) buscar_nodo(nodo.blk_indirect[i]);
		sem_wait(&mutex_datos);
		memcpy(punteros_a_datos,
				buscar_nodo(nodo.blk_indirect[indice_indirectos]),
				TAMANIO_BLOQUE);
		sem_post(&mutex_datos);

		while (indice_directos < 1024
				&& !no_existe_puntero(punteros_a_datos[indice_directos])) {
			//nodo_datos = buscar_nodo(punteros_a_datos[j]);
			sem_wait(&mutex_mapa);
			usado = bitarray_test_bit(bitmap,
					nodo.blk_indirect[indice_indirectos]);
			sem_post(&mutex_mapa);
			if (!usado) {
				logger_info(logger,
						"Bloque %i no utilizado pero intenta borrarse. ind",
						punteros_a_datos[indice_directos]);
			}
			sem_wait(&mutex_mapa);
			bitarray_clean_bit(bitmap, punteros_a_datos[indice_directos]);
			sem_post(&mutex_mapa);
			indice_directos++;

		}
		sem_wait(&mutex_mapa);
		usado = bitarray_test_bit(bitmap, nodo.blk_indirect[indice_indirectos]);
		sem_post(&mutex_mapa);
		if (!usado) {
			logger_info(logger, "Bloque %i no utilizado pero intenta borrarse",
					nodo.blk_indirect[indice_indirectos]);
		}
		sem_wait(&mutex_mapa);
		bitarray_clean_bit(bitmap, nodo.blk_indirect[indice_indirectos]);
		sem_post(&mutex_mapa);
		indice_directos = 0;
		indice_indirectos++;
	}
	return EXIT_SUCCESS;
}
