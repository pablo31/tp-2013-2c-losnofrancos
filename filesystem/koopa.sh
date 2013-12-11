#!/bin/bash
#Este shell script ejecuta koopa contra el directorio de GRASA especificado y otro directorio para comparacion.
clear

directorio_grasa="grasa" #El directorio donde esta montado grasa.
koopa="koopa-2c2013/Release/./koopa" #El path a koopa.
scripts="scripts.sh" #El path al archivo de scripts

echo "Ejecutando koopa."
echo "Directorio grasa: $directorio_grasa"
echo "Koopa: $koopa"
echo "Scripts: $scripts"

#Modo texto
#$koopa $directorio_grasa $scripts --text

#Modo GUI
$koopa $directorio_grasa $scripts 