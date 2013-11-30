#ifndef FS_OPERATIONS
#define FS_OPERATIONS

#define TAMANIO_BLOQUE 		  4096

#include <semaphore.h>
#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <fcntl.h>
#include "../libs/common/collections/list.h"

int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fs_mkdir(const char *path, mode_t mode);
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int fs_opendir(const char *path, struct fuse_file_info *fi);
int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fs_unlink(const char *path);
int fs_rmdir (const char *path);
int fs_open(const char *path, struct fuse_file_info *fi);
int fs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int fs_getattr(const char *path, struct stat * nose);
int fs_truncate(const char * path, off_t offset);
int fs_setattr(const char * path, struct stat *stat);

int agregar_nodo(const GFile nodo);
int borrar_nodo(const uint32_t bloque);
int buscar_bloque_nodo(char* path, uint32_t *bloque);
int buscar_bloque_por_padre(char *padre, uint32_t bloque_padre, uint32_t *bloque);
int buscar_nodos_por_padre(uint32_t bloque_padre, void * buffer, fuse_fill_dir_t filler);
int liberar_espacio(uint32_t bloque);
uint cargar_datos(GFile archivo, char* buffer, size_t size, off_t offset);
int guardar_datos(GFile* archivo, const char* buffer, size_t size, off_t inicio);
void liberar_subpath(char** subpath);
#endif
