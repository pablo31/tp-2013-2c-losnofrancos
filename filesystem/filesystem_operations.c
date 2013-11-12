#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
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
extern GFile nodos[1024];

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
	//logger_info(logger, "Abro archivo:");
	logear_path("fs_read", path);

	//leer archivo desde offset hasta size y guardar en buff
	return 0;
}

/*
 Create a directory

 Note that the mode argument may not have the type specification bits set, i.e. S_ISDIR(mode) can be false. To obtain the correct
 directory type bits use mode|S_IFDIR
 */
int fs_mkdir(const char *path, mode_t mode) {
	char** subpath;
	char *directorio = NULL;
	int i = 0;
	int err = 0;
	GFile nodo;
	uint32_t bloque_padre = 0;

	//logger_info(logger, "Creo directorio:");
	logear_path("fs_mkdir", path);

	subpath = string_split(path, "/");
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
	//nodo.fname = strdup(directorio);
	//strcpy(nodo.fname, directorio);
	nodo.state = 2;

	return agregar_nodo(nodo);

}

/*
 Create and open a file

 If the file does not exist, first create it with the specified mode, and then open it.

 If this method is not implemented or under Linux kernel versions earlier than 2.6.15, the mknod() and open() methods will be called instead.
 */
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	//logger_info(logger, "Creo archivo:");
	logear_path("fs_create", path);
	;
	return 0;
}

/*
 Open directory

 Unless the 'default_permissions' mount option is given, this method should check if opendir is permitted for this directory. Optionally opendir may also return an arbitrary filehandle in the fuse_file_info structure, which will be passed to readdir, closedir and fsyncdir.

 */
int fs_opendir(const char *path, struct fuse_file_info *fi) {
	logear_path("fs_opendir", path);
	int err = 0;
	uint32_t bloque;
	err = buscar_bloque_nodo(path, bloque);
	if (!err) {
	return 0;
} else {
	return -ENOENT;
}
/*
 logger_info(logger,"");
 logger_info(logger,"\tFile info flags:'%s'",fi->flags);
 logger_info(logger,"\tFile info fh_old:'%lu'",fi->fh_old);
 logger_info(logger,"\tFile info writepage:'%i'",fi->writepage);
 logger_info(logger,"\tFile info direct_io:'%u'",fi->direct_io);
 logger_info(logger,"\tFile info keep_cache:'%u'",fi->keep_cache);
 logger_info(logger,"\tFile info flush:'%u'",fi->flush);
 logger_info(logger,"\tFile info nonseekable:'%u'",fi->nonseekable);
 logger_info(logger,"\tFile info padding:'%u'",fi->padding);
 logger_info(logger,"\tFile info fh:'%u'",fi->fh);
 logger_info(logger,"\tFile info lock_owner:'%u'",fi->lock_owner);
 */

return 0;
}
/*
 Write data to an open file

 Write should return exactly the number of bytes requested except on error. An exception to this is when the 'direct_io' mount option is specified (see read operation).
 */
int fs_write(const char *path, const char *buf, size_t size, off_t offset,
	struct fuse_file_info *fi) {
//logger_info(logger, "Escribo archivo:");
logear_path("fs_write", path);
return 0;
}

/*
 Remove a file
 */
int fs_unlink(const char *path) {
//logger_info(logger, "Elimino archivo:");
logear_path("fs_unlink", path);

return 0;
}

/*
 Remove a directory
 */
int fs_rmdir(const char *path) {
int err = 0;
uint32_t bloque = 0;

//logger_info(logger, "Elimino directorio");
logear_path("fs_rmdir", path);

err = buscar_bloque_nodo(path, &bloque);

return borrar_nodo(bloque);

//actualizar nodos en archivo?

}

int fs_getattr(const char * path, struct stat *stat) {
//logger_info(logger, "get attributes");
logear_path("fs_getattr", path);

memset(stat, 0, sizeof(struct stat));
stat->st_size = 5;
//stat->st_atim.tv_sec = stat->st_ctim.tv_sec = stat->st_mtim.tv_sec = time(NULL);
stat->st_uid = "utnso";
stat->st_gid = "utnso";
stat->st_mode = S_IFDIR;
stat->st_nlink = 2;

return 0;
}

int fs_open(const char *path, struct fuse_file_info *fi) {
//logger_info(logger, "Abrir");
logear_path("fs_open", path);

if ((fi->flags & 3) != O_RDONLY)
	return -EACCES;

return 0;
}

int fs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
	off_t offset, struct fuse_file_info *fi) {
int i = 0;
t_list *contenido;
uint32_t bloque_padre;
int err = 0;
char *nombre;
//logger_info(logger, "Leer directorio");
logear_path("fs_readdir", path);

//Agrego las opciones . y .. por defecto
filler(buffer, ".", NULL, 0);
filler(buffer, "..", NULL, 0);

// Busco todo el contenido del directorio
contenido = list_create;
err = buscar_bloque_nodo(path, bloque_padre);
if (!err) {
	buscar_nodos_por_padre(bloque_padre, contenido);
	if (!err) {
		for (i = 0; i < (*contenido).elements_count; i++) {
			nombre = (char*) list_get(contenido, 1);
			filler(buffer, nombre, NULL, 0);
		};
	} else {
		return -ENOENT;
	}
} else {
	return -ENOENT;
}

//busco los demas directorio y los agrego a buffer con filler

return 0;
}
//--------------------------
// Funciones auxiliares:   |
//--------------------------
int agregar_nodo(const GFile nodo) {
int fin = 0;
int i = 0;

while (!fin && i < GFILEBYTABLE) {
	if (nodos[i].state == 0) {
		nodos[i] = nodo;
		fin = 1;
	}
	i++;

}

if (fin) {
	return EXIT_SUCCESS;
} else {
	return EXIT_FAILURE;
}
}

int borrar_nodo(const uint32_t bloque) {
if (nodos[bloque].state != 0) {
	nodos[bloque].state = 0;
	return EXIT_SUCCESS;
} else {
	return EXIT_FAILURE;
}
}
/* Busco el nodo con nombre "fname" y cuyo padre es el bloque "bloque_padre"
 * y lo devuelvo en "bloque"
 */
int buscar_bloque_por_padre(char *fname, uint32_t bloque_padre,
	uint32_t *bloque) {
int encontrado = 0;
uint32_t i = 0;

while (!encontrado && i < GFILEBYTABLE) {
	if (strncmp(nodos[i].fname, fname, GFILENAMELENGTH)
			&& nodos[i].parent_dir_block == fname) {
		*bloque = i;
		encontrado = 1;
	}
	i++;
}
if (encontrado)
	return EXIT_SUCCESS;
else
	return EXIT_FAILURE;

}
// con el path completo busco el bloque en donde esta guardado ese nodo y lo devuelvo
int buscar_bloque_nodo(const char* path, uint32_t *bloque) {
char** subpath;
int i = 0;
int err = 0;
uint32_t bloque_padre = 0;
subpath = string_split(path, "/");
while (subpath[i] != NULL ) {
	bloque_padre = bloque;
	err = buscar_bloque_por_padre(subpath[i], bloque_padre, bloque);
	if (err) {
		free(subpath);
		return EXIT_FAILURE;

	}
	i++;
	free(subpath[i]);
};
free(subpath);
return EXIT_SUCCESS;
}

// Busca todos los nodos que tienen como padre al bloque "bloque_padre"
int buscar_nodos_por_padre(uint32_t bloque_padre, t_list *contenido) {
int i;
char * nombre;
for (i = 0; i < GFILEBYTABLE; i++) {
	if (nodos[i].parent_dir_block == bloque_padre) {
		nombre = malloc(sizeof(nodos[i].fname));
		nombre[sizeof(nodos[i].fname) - 1] = '\0';
		memcpy(nombre, nodos[i].fname, sizeof(nodos[i].fname));
		list_add(contenido, nombre);
	}
}
return EXIT_SUCCESS;
}

