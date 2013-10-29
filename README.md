tp-2013-2c-losnofrancos
=======================
"Desarrollando para usted desde hace mas de un año ;D"

Compilar todo con make

	make
	
Ejecutar distintas pruebas que implementan pequeñas partes de la biblioteca libs

	./prueba_cliente.sh
	./prueba_servidor.sh
	
	./prueba_threads.sh
	./prueba_semaforos.sh
	
	./prueba_notificador.sh archivo_a_monitorear
	
	./prueba_senales.sh
	
	./prueba_listas.sh
	./prueba_saltos.sh
	./prueba_variables.sh

Ejecutar los distintos componentes del sistema

	./plataforma.sh
	./personaje.sh config log
	./nivel.sh config log

Opcionalmente, se puede limpiar el workspace

	make clean
