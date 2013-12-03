#!/bin/bash
#x-terminal-emulator es el proceso que ejecuta su terminal. No tengo idea cual es la de su maquina
#--geometry=100x30 es el tamaño que le doy a la terminal cuando se crea
#-e es CREO para decirle que ejecute el siguiente argumento (osea nuestro proceso)

x-terminal-emulator --geometry=100x30 -e ./plataforma.sh configs/plataforma.cfg

x-terminal-emulator --geometry=100x30 -e ./nivel.sh configs/nivel1.cfg
x-terminal-emulator --geometry=100x30 -e ./nivel.sh configs/nivel2.cfg
x-terminal-emulator --geometry=100x30 -e ./nivel.sh configs/nivel3.cfg

x-terminal-emulator --geometry=100x30 -e ./personaje.sh configs/luigi.cfg


#Magia , como funciona? Magia. Belleza.

#Hagan sus tests a partir de este ejemplo , espero que les ahorre horas de testeo.