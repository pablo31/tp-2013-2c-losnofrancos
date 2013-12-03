#!/bin/bash

sed -i 's/orquestador=127.0.0.1:27016/orquestador=127.0.0.1:27015/g' *.cfg

#find . -name '*.cfg' -print0 | xargs -0 sed -i 's/orquestador=127.0.0.1:27015/orquestador=127.0.0.1:27016/g'
