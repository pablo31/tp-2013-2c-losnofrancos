#!/bin/bash
clear

#aca especificamos donde se va a montar el filesystem
mount_point="/media/nagel/grasafs/fs" 
echo "Montando grasa en $mount_point"
./fuse_example $mount_point -f
