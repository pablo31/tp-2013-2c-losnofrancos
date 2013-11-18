all:
	clear 
	@echo "   >>> COMPILANDO PROCESO FILESYSTEM"
	@gcc -Wall -g -ggdb -c ../libs/common/*.c ../libs/common/collections/*.c
	@ar rcs libcommon.a bitarray.o  config.o  dictionary.o  error.o  list.o  log.o  process.o  queue.o  string.o  temporal.o  txt.o
	@gcc -Wall -g -ggdb -c ../libs/logger/logger.c
	@ar rcs liblog.a logger.o string.o log.o error.o temporal.o  txt.o process.o
	@gcc -Wall -g -ggdb filesystem.c filesystem_operations.c  -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 liblog.a libcommon.a -o filesystem -lpthread -lfuse -lm
	@rm *.a *.o