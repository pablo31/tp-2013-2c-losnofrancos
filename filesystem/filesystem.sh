#!/bin/bash
clear

#aca especificamos donde se va a montar el filesystem
archivo="disco.bin" 
directorio="grasa"
echo "Montando grasa desde $archivo en $directorio"
echo ""


#debug
#valgrind --tool=exp-sgcheck ./filesystem $archivo $directorio 
./filesystem $archivo $directorio 