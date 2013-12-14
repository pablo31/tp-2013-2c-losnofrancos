#!/bin/bash

echo "Generando disco.bin"
dd if=/dev/urandom of=disco.bin bs=1024 count=20480

echo "Formateando disco bin con grasa-format"
utils/grasa-format disco.bin
