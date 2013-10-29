Proceso FileSystem 
===================
Uso

1) Compilar todo con make

	make
	
2) Crear el archivo de grasa SI NO ESTA CREADO ANTERIORMENTE

	./grasa-format disco.bin

3) Abrir 'filesystem.sh' y comprobar que los parametros de ejecucion

	nano filesystem.sh

4) Ejecutar el shell script filesystem.sh

	./filesystem.sh

5) Comprobar que el proceso este manejando el acceso a la carpeta

	mount
	** validar que se encuentre una linea similar a 
	** filesystem on /home/nagel/code/tp-2013-2c-losnofrancos/filesystem/grasa type fuse.filesystem (rw,nosuid,nodev,user=nagel)

6) Desmontar el directorio cuando no se necesite usar mas 
	
	fusermount -u grasa
	**en el ejemplo el directorio es 'grasa'