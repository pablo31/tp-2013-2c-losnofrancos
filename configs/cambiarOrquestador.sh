#!/bin/bash

sed -i 's/Plataforma=127.0.0.1:27016/Plataforma=127.0.0.1:27015/g' *.cfg

#find . -name '*.cfg' -print0 | xargs -0 sed -i 's/Plataforma=127.0.0.1:27015/Plataforma=127.0.0.1:27016/g'
