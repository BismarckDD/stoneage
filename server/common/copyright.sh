#!/bin/sh
OUT=./copyright.h
echo "#define SERVER_VERSION \" SaServer 8.0 "$(date +%Y��%m��%d��%H:%M:%S)"\"" > $OUT
