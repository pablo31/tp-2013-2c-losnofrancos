grasa-tools
===========

Herramientas de Gestión del Gran Sistema de Archivos


Usage:

1) Crear un archivo para disco binario. 

	dd if=/dev/urandom of=disco.bin bs=1024 count=10240
	
	** Modificar "count" para la cantidad de kilobytes necesarios.

2) Formatear el disco binario

	./grasa-format disco.bin

3) Hacer un dump de sus estructuras internas.

	./grasa-dump disco.bin



