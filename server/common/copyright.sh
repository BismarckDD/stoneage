#!/bin/sh
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
OUT=./copyright.h
echo "#define SERVER_VERSION \" SaServer 8.0 "$(date +%YÄê%mÔÂ%dÈÕ%H:%M:%S)"\"" > $OUT
