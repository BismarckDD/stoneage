#!/bin/sh
OUT=./copyright.h
echo "#define SERVER_VERSION \" Saserver 8.0 "$(date +%Y��%m��%d��%H:%M:%S)"\n\"" > $OUT
