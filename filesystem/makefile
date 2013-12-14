all:
	clear 
	@echo "   >>> COMPILANDO PROCESO FILESYSTEM"
	@gcc -Wall -g -ggdb -c ../libs/common/*.c ../libs/common/collections/*.c
	@ar rcs libcommon.a bitarray.o  config.o  dictionary.o  error.o  list.o  log.o  process.o  queue.o  string.o  temporal.o  txt.o
	@gcc -Wall -g -ggdb -c ../libs/logger/logger.c
	@ar rcs liblog.a logger.o string.o log.o error.o temporal.o  txt.o process.o
	@gcc -Wall -g -ggdb filesystem.c filesystem_operations.c  -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 liblog.a libcommon.a -o filesystem -lpthread -lfuse -lm
	@rm *.a *.o
	#@echo "   >>> COMPILANDO PROCESO Koopa"
	#@cd koopa-2c2013/Release/
	#@make all 
	#@cd ../..
	#@echo "   >>> COMPILANDO PROCESO Massive File Creator"
	#@cd sisoputnfrba-massive-file-creator-3c89d14
	#@gcc massive-file-creator.c -o mfc -lcrypto -lpthread
	#@cd ..