export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I. -I./include
CFLAGS=-m32 -w -O3 -g -rdynamic $(INCFLAGS) -pipe -Wall

ifeq ($(shell uname),CYGWIN_NT-5.1)
	SQLSRC=sasql_cygwin.c
	SQL=-lodbc32 -lz -lpthread
	TARGET=./saacjt
endif
ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz -lpthread
	TARGET=./saacjt
endif

RPCSRC = saacproto_serv.c saacproto_util.c 
RPCINCLUDE = saacproto_serv.h saacproto_util.h mac.h
RPCNOUSE = saacproto_cli.c saacproto_cli.h

SRC = main.c recv.c util.c char.c db.c mail.c acfamily.c lock.c attestation.c md5.c $(SQLSRC) $(RPCSRC)
OBJ=$(SRC:.c=.o)

${TARGET}: copyright $(RPCSRC) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(SQL)
	$(RM) $(OBJ)

copyright:
	$(SHELL) ./copyright.sh

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

rpc : ${RPCHTML} ${RPCGEN}
	$(PERL) ${RPCGEN} ${RPCHTML} -h . -o .
	rm -f ${RPCNOUSE}

