all:
	clear 
	@echo "   >>> COMPILANDO PROCESO FILESYSTEM"
	gcc -Wall filesystem.c -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -o filesystem -lpthread -lfuse