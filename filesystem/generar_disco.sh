#!/bin/bash

echo "generando disco.bin"
dd if=/dev/urandom of=disco.bin bs=1024 count=10240

echo "formateando disco bin con grasa-format"
utils/grasa-format disco.bin
