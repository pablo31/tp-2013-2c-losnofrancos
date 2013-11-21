LosNoFrancos
============
"Desarrollando para usted desde hace mas de un año ;D"

![alt tag](https://raw.github.com/pablo31/tp-2013-2c-losnofrancos/master/doc/img.png)
TP para Sistemas Operativos 2c2013

Proyectos de Eclipse incluidos:

- tp-libs
- tp-plataforma
- tp-nivel
- tp-personaje
- tp-filesystem

Proyectos de NetBeans incluidos (desactualizados):

- libs
- plataforma
- nivel
- personaje


Instalacion
===========

La compilacion se debe realizar desde consola haciendo

	make

Extensiones predefinidas de los archivos

* .sh => ejecutables
* .a => bibliotecas
* .cfg => archivos de configuracion
* .log => archivos de logueo


Ejecutables
===========

Los distintos componentes del sistema son:

	./plataforma.sh <config_file>
	./nivel.sh <config_file>
	./personaje.sh <config_file>

Los archivos de configuracion se puede encontrar en

	./configs/


Pruebas
=======

Luego de compilar se tiene acceso a distintos ejecutables que implementan distintas bibliotecas de tp-libs

	./prueba_cliente.sh
	./prueba_servidor.sh
	
	./prueba_threads.sh
	./prueba_semaforos.sh
	
	./prueba_notificador.sh <archivo_a_monitorear>
	
	./prueba_senales.sh
	
	./prueba_listas.sh
	./prueba_saltos.sh
	./prueba_variables.sh


Autores
=======

Andres
Jorge
Antonella
Braulio
Pablo