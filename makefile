# Parametros que recibe gcc:
# 	-Wall : Warning all Basicamente pone Warnings extra el compilador.
#	-g 	  : Genera informacion para debugear con GDB
#	-ggdb : Genera MAS informacion para debugear con GDB
# make
# Esta opcion es la default del make cuando lo ejecutamos.
all: 
	@clear

	@echo "Los No Francos 2013"
	@echo "'Desarrollando para usted desde hace más de un año'"
	@echo ""
	@sleep 1

	@echo "# BIBLIOTECAS"
	@echo ""

	@echo "   >>> COMPILANDO BIBLIOTECA COMMON"
	@gcc -Wall -g -ggdb -c libs/common/*.c libs/common/collections/*.c libs/vector/vector2.c libs/error/exception.c
	@ar rcs libcommon.a bitarray.o  config.o  dictionary.o  error.o  list.o  log.o  process.o  queue.o  string.o  temporal.o  txt.o vector2.o exception.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA LOG"
	@gcc -Wall -g -ggdb -c libs/logger/logger.c
	@ar rcs liblog.a logger.o string.o log.o error.o temporal.o  txt.o process.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA SOCKET"
	@gcc -Wall -g -ggdb -c libs/socket/socket.c libs/socket/socket_utils.c libs/socket/package_serializers.c
	@ar rcs libsck.a socket.o socket_utils.o package_serializers.o logger.o string.o log.o error.o temporal.o  txt.o process.o vector2.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA THREAD Y MUTEX"
	@gcc -Wall -g -ggdb -c libs/thread/thread.c libs/thread/mutex.c libs/command/command.c libs/command/arguments.c -lpthread
	@ar rcs libth.a mutex.o thread.o command.o arguments.o list.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA MULTIPLEXOR"
	@gcc -Wall -g -ggdb -c libs/multiplexor/multiplexor.c libs/command/command.c libs/command/arguments.c
	@ar rcs libmpx.a multiplexor.o command.o arguments.o list.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA NOTIFIER"
	@gcc -Wall -g -ggdb -c libs/notifier/notifier.c
	@ar rcs libntf.a notifier.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA SIGNAL"
	@gcc -Wall -g -ggdb -c libs/signal/signal.c libs/command/command.c libs/command/arguments.c
	@ar rcs libsgn.a signal.o command.o arguments.o list.o
	
	@echo ""
	@echo "   >>> COMPILANDO BIBLIOTECA COLLECTION"
	@gcc -Wall -g -ggdb -c libs/collection/round.c
	@ar rcs libctn.a round.o list.o
	
	@echo ""
	@echo "# PRUEBAS"
	@echo ""

	@echo "   >>> COMPILANDO PRUEBA CLIENTE"
	@gcc -Wall -g -ggdb pruebas/cliente.c libcommon.a libsck.a liblog.a libsgn.a -o prueba_cliente.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA SERVIDOR (MULTIPLEXOR)"
	@gcc -Wall -g -ggdb pruebas/server.c libcommon.a libsck.a liblog.a libsgn.a libmpx.a -o prueba_servidor.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA DE LISTAS"
	@gcc -Wall -g -ggdb pruebas/listas.c libcommon.a libctn.a -o prueba_listas.pv
	@echo ""
	
	@echo "   >>> COMPILANDO PRUEBAS THREADS Y SEMAFOROS"
	@gcc -Wall -g -ggdb pruebas/threads.c libs/error/exception.c libth.a -o prueba_threads.pv -lpthread
	@gcc -Wall -g -ggdb pruebas/semaforos.c libth.a -o prueba_semaforos.pv -lpthread
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA DE SALTOS"
	@gcc -Wall -g -ggdb pruebas/saltos.c -o prueba_saltos.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA DE SENALES"
	@gcc -Wall -g -ggdb pruebas/senales.c libsgn.a -o prueba_senales.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA DE NOTIFICADOR"
	@gcc -Wall -g -ggdb pruebas/notificador.c libsgn.a libntf.a -o prueba_notificador.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA DE VARIABLES"
	@gcc -Wall -g -ggdb pruebas/variables.c libs/error/exception.c -o prueba_variables.pv
	
	@echo ""
	@echo "   >>> COMPILANDO PRUEBA X"
	@gcc -Wall -g -ggdb pruebas/intro.c libcommon.a -o prueba_x.pv -lncurses
	
	@echo ""
	@echo "# PROCESOS"
	@echo ""
	
	@echo "   >>> COMPILANDO PROCESO PERSONAJE"
	@gcc -Wall -g -ggdb personaje/personaje.c personaje/personaje_config.c libcommon.a libctn.a libsck.a liblog.a libth.a libsgn.a -o personaje.pv -lpthread
	
	@echo ""
	@echo "   >>> COMPILANDO PROCESO PLATAFORMA"
	@gcc -Wall -g -ggdb plataforma/plataforma.c plataforma/orquestador.c plataforma/planificador.c libcommon.a libctn.a libsck.a liblog.a libth.a libmpx.a libsgn.a -o plataforma.pv -lpthread
	
	@echo ""
	@echo "   >>> COMPILANDO PROCESO NIVEL"
	@gcc -Wall -ggdb nivel/nivel_ui.c nivel/nivel_configuracion.c nivel/nivel.c nivel/enemigo.c nivel/verificador_deadlock.c -lncurses libcommon.a libsck.a liblog.a libmpx.a libth.a libntf.a libsgn.a -lpthread -o nivel.pv
	
	@echo ""
	@echo ""
	@echo "<<< Limpiando archivos output"
	@rm *.o
	@echo "<<< Eliminando librerias"
	@rm *.a
	@echo ""

# make clean	
# Esta opcion nos borra las librerias generadas y los ejecutables. Nos sirve para cuando nos quedo a medio masticar una compilacion
clean:
	@clear
	@echo "Limpiando ejecutables"
	@rm *.pv
	@echo "Limpiando archivos de log"
	@rm *.log
	@echo "Limpiando archivos output"
	@rm *.o
	@echo "Limpiando librerias"
	@rm *.a
	@echo ""

# make install
# Esta opcion va a instalar el programa en el linux de testeo. Basicamente va a copiar todo lo necesario a /usr/bin
install:
#TODO:hacerlo