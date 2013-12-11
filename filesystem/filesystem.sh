#!/bin/bash
clear

archivo="disco.bin" #El archivo que contiene el FS Grasa.
directorio="grasa" #El directorio donde se va a montar Grasa

echo "Montando grasa desde $archivo en $directorio"
rm grasa.log #borro el archivo de log anterior asi no se hace muy grande


#  Debug
#valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./filesystem $archivo $directorio
#valgrind --tool=exp-sgcheck ./filesystem $archivo $directorio 

#  Release
./filesystem $archivo $directorio

echo "Desmonar con 'fusermount -u $directorio'"