#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "grasa.h"

struct t_runtime_options {
	//char* welcome_msg;
} runtime_options;

enum {
	KEY_VERSION,
	KEY_HELP,
};

/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
static struct fuse_opt fuse_options[] = {
		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

/*
Read data from an open file

Read should return exactly the number of bytes requested except on EOF or error, otherwise the rest of the data will be substituted 
with zeroes. An exception to this is when the 'direct_io' mount option is specified, in which case the return value of the read system 
call will reflect the return value of this operation.
*/
static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	return 1;
}

/*
Create a directory

Note that the mode argument may not have the type specification bits set, i.e. S_ISDIR(mode) can be false. To obtain the correct 
directory type bits use mode|S_IFDIR
*/
static int fs_mkdir(const char *path, mode_t mode){
	return 1;	
}

/*
Create and open a file

If the file does not exist, first create it with the specified mode, and then open it.

If this method is not implemented or under Linux kernel versions earlier than 2.6.15, the mknod() and open() methods will be called instead.
*/
static int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
	return 1;	
}


/*
Open directory

Unless the 'default_permissions' mount option is given, this method should check if opendir is permitted for this directory. Optionally opendir may also return an arbitrary filehandle in the fuse_file_info structure, which will be passed to readdir, closedir and fsyncdir.

*/
static int fs_opendir(const char *path, struct fuse_file_info *fi){
	return 1;	
}
/*
Write data to an open file

Write should return exactly the number of bytes requested except on error. An exception to this is when the 'direct_io' mount option is specified (see read operation).
*/
static int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	return 1;
}

/*
Remove a file
*/
static int fs_unlink(const char *path){
	//Remove a file
	return 1;
}

static int fs_rmdir (const char *path){
	return 1;
}

static struct fuse_operations fuse_operations = {
		.read = fs_read,
		.mkdir = fs_mkdir,
		.create = fs_create,
		.opendir = fs_opendir,
		.write = fs_write,
		.unlink = fs_unlink,
		.rmdir = fs_rmdir,
		
};
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }

// Dentro de los argumentos que recibe nuestro programa obligatoriamente
// debe estar el path al directorio donde vamos a montar nuestro FS
int main(int argc, char *argv[]) {
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		/** error parsing options */
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	return fuse_main(args.argc, args.argv, &fuse_operations, NULL);
}

