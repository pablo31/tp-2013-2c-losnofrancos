#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
//#include <fcntl.h>
#include "grasa.h"

/*
Read data from an open file

Read should return exactly the number of bytes requested except on EOF or error, otherwise the rest of the data will be substituted 
with zeroes. An exception to this is when the 'direct_io' mount option is specified, in which case the return value of the read system 
call will reflect the return value of this operation.
*/
int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	printf("fs_read\n" );
	return 0;
}

/*
Create a directory

Note that the mode argument may not have the type specification bits set, i.e. S_ISDIR(mode) can be false. To obtain the correct 
directory type bits use mode|S_IFDIR
*/
int fs_mkdir(const char *path, mode_t mode){
	printf("fs_read\n" );
	return 0;	
}

/*
Create and open a file

If the file does not exist, first create it with the specified mode, and then open it.

If this method is not implemented or under Linux kernel versions earlier than 2.6.15, the mknod() and open() methods will be called instead.
*/
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi){
	printf("fs_read\n" );
return 0;	
}


/*
Open directory

Unless the 'default_permissions' mount option is given, this method should check if opendir is permitted for this directory. Optionally opendir may also return an arbitrary filehandle in the fuse_file_info structure, which will be passed to readdir, closedir and fsyncdir.

*/
int fs_opendir(const char *path, struct fuse_file_info *fi){
	printf("fs_read\n" );
return 0;	
}
/*
Write data to an open file

Write should return exactly the number of bytes requested except on error. An exception to this is when the 'direct_io' mount option is specified (see read operation).
*/
int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	printf("fs_read\n" );
return 0;
}

/*
Remove a file
*/
int fs_unlink(const char *path){
	//Remove a file
	printf("fs_read\n" );
return 0;
}

int fs_rmdir (const char *path){
	printf("fs_read\n" );
return 0;
}
