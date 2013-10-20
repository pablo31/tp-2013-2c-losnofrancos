#ifndef FS_OPERATIONS
#define FS_OPERATIONS

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <fcntl.h>

int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fs_mkdir(const char *path, mode_t mode);
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int fs_opendir(const char *path, struct fuse_file_info *fi);
int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fs_unlink(const char *path);
int fs_rmdir (const char *path);

#endif