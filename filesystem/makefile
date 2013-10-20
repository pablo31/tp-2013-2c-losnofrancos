all:
	clear 
	@echo "   >>> COMPILANDO PROCESO FILESYSTEM"
	gcc -Wall -g -ggdb -c ../libs/common/*.c ../libs/common/collections/*.c
	ar rcs libcommon.a bitarray.o  config.o  dictionary.o  error.o  list.o  log.o  process.o  queue.o  string.o  temporal.o  txt.o
	clang  -Wall  filesystem_operations.c filesystem.c -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 libcommon.a -o filesystem -lpthread -lfuse
	rm *.a