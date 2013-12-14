#!/bin/bash
for i in {1..5}
do
	touch $i.txt
	for j in {1..10}
	do
		mkdir -p dir$i/dir$j
	done
done

dd if=/dev/zero of=dir1/archivo.zero bs=1 count=102400
dd if=/dev/zero of=dir3/archivo.zero bs=4095 count=3

echo "HOLA MUNDO" > dir2/hola.txt

cp /bin/bash archivo.bin
dd if=archivo.bin of=dir5/dir5/b.bin bs=1 count=5000