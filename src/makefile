# Parametros que recibe gcc:
# 	-Wall : Warning all Basicamente pone Warnings extra el compilador.
#	-g 	  : Genera informacion para debugear con GDB
#	-ggdb : Genera MAS informacion para debugear con GDB
# make
# Esta opcion es la default del make cuando lo ejecutamos.
all: 
	clear

	@echo "Es Lo Que Hay 2013 >>"
	@echo ""

	@echo "# LIBRERIAS"
	@echo ""

	@echo "   >>> COMPILANDO LIBRERIA COMMON"
	gcc -Wall -g -ggdb -c libs/common/*.c libs/common/collections/*.c
	ar rcs libcommon.a bitarray.o  config.o  dictionary.o  error.o  list.o  log.o  process.o  queue.o  string.o  temporal.o  txt.o

	@echo "   >>> COMPILANDO LIBRERIA LOG"
	gcc -Wall -g -ggdb -c libs/logger/logger.c
	ar rcs liblog.a logger.o string.o log.o error.o temporal.o  txt.o process.o

	@echo "   >>> COMPILANDO LIBRERIA SOCKET"
	gcc -Wall -g -ggdb -c libs/socket/socket.c libs/socket/socket_utils.c
	ar rcs liblog.a socket.o socket_utils.o logger.o string.o log.o error.o temporal.o  txt.o process.o

	@echo "   >>> COMPILANDO LIBRERIA THREAD Y MUTEX"
	gcc -Wall -g -ggdb -c libs/thread/thread.c libs/thread/mutex.c libs/command/command.c libs/command/arguments.c -lpthread
	ar rcs libth.a mutex.o thread.o command.o arguments.o list.o

	@echo "   >>> COMPILANDO LIBRERIA MULTIPLEXOR"
	gcc -Wall -g -ggdb -c libs/multiplexor/multiplexor.c libs/command/command.c libs/command/arguments.c
	ar rcs libmpx.a multiplexor.o list.o command.o arguments.o list.o

	@echo "   >>> COMPILANDO LIBRERIA SIGNAL"
	gcc -Wall -g -ggdb -c libs/signal/signal.c libs/command/command.c libs/command/arguments.c
	ar rcs libsgn.a signal.o command.o arguments.o list.o

	@echo ""
	@echo "# PRUEBAS"
	@echo ""

	@echo "   >>> COMPILANDO PRUEBA CLIENTE"
	gcc -Wall -g -ggdb pruebas/cliente.c libcommon.a liblog.a libsgn.a -o prueba_cliente.sh
	
	@echo "   >>> COMPILANDO PRUEBA SERVIDOR (MULTIPLEXOR)"
	gcc -Wall -g -ggdb pruebas/server.c libcommon.a liblog.a libsgn.a libmpx.a -o prueba_servidor.sh

	@echo "   >>> COMPILANDO PRUEBA DE LISTAS"
	gcc -Wall -g -ggdb pruebas/listas.c libcommon.a -o prueba_listas.sh

	@echo "   >>> COMPILANDO PRUEBAS THREADS Y SEMAFOROS"
	gcc -Wall -g -ggdb pruebas/threads.c libth.a -o prueba_threads.sh -lpthread
	gcc -Wall -g -ggdb pruebas/semaforos.c libth.a -o prueba_semaforos.sh -lpthread

	@echo "   >>> COMPILANDO PRUEBA DE SALTOS"
	gcc -Wall -g -ggdb pruebas/saltos.c -o prueba_saltos.sh

	@echo "   >>> COMPILANDO PRUEBA DE SENALES"
	gcc -Wall -g -ggdb pruebas/senales.c libsgn.a -o prueba_senales.sh

	@echo "   >>> COMPILANDO PRUEBA DE VARIABLES"
	gcc -Wall -g -ggdb pruebas/variables.c -o prueba_variables.sh
	
	@echo ""
	@echo "# PROCESOS"
	@echo ""
	
	@echo "   >>> COMPILANDO PROCESO PERSONAJE"
	#TODO
	
	@echo "   >>> COMPILANDO PROCESO PLATAFORMA"
	#TODO

	@echo ""
	@echo "   <<< Limpiando archivos output"
	rm *.o
	@echo "   <<< Eliminando librerias"
	rm *.a

# make clean	
# Esta opcion nos borra las librerias generadas y los ejecutables. Nos sirve para cuando nos quedo a medio masticar una compilacion
clean:
	clear #esto hace que se 'limpie' la terminal 
	@echo "Limpiando ejecutables"
	rm *.sh		
	@echo ""

# make install
# Esta opcion va a instalar el programa en el linux de testeo. Basicamente va a copiar todo lo necesario a /usr/bin
install:
#TODO:hacerlo