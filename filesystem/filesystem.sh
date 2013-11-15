#!/bin/bash
clear


archivo="disco.bin" #El archivo que contiene el FS Grasa.
directorio="grasa" #El directorio donde se va a montar Grasa


echo "Montando grasa desde $archivo en $directorio"

#debug
rm grasa.log #borro el archivo de log anterior asi no se hace muy grande
#valgrind --tool=memcheck --leak-check=full ./filesystem $archivo $directorio 
#valgrind --tool=exp-sgcheck ./filesystem $archivo $directorio 

./filesystem $archivo $directorio -d

echo "Desmonar con 'fusermount -u $directorio'"