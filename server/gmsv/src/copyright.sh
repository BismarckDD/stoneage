#!/bin/sh
OUT=./copyright.h
echo "#ifdef _VERSION_NEW" > $OUT
echo "#define SERVER_VERSION \"SA Special Edition "$(date +%Y年%m月%d日%H:%M:%S)"\"" >> $OUT
echo "#else" >> $OUT
echo "#ifdef _VERSION_25" >> $OUT
echo "#define SERVER_VERSION \"SAServer 2.5 "$(date +%Y年%m月%d日%H:%M:%S)"\"" >> $OUT
echo "#else" >> $OUT
echo "#define SERVER_VERSION \"SAServer 8.0 "$(date +%Y年%m月%d日%H:%M:%S)"\"" >> $OUT
echo "#endif" >> $OUT
echo "#endif" >> $OUT
