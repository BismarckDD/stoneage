export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I../common -I./include
CFLAGS=-m32 -w -O3 -g -pipe -Wall $(INCFLAGS)

ifeq ($(shell uname),CYGWIN_NT-5.1)
	SQLSRC=sasql_cygwin.c
	SQL=-lodbc32 -lz -lpthread
	TARGET=./saacjt
endif
ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz -lpthread
	TARGET=../../csa/saac/saacjt
endif

RPCSRC = saacproto_serv.c saacproto_util.c 
RPCINCLUDE = saacproto_serv.h saacproto_util.h
RPCNOUSE = saacproto_cli.c saacproto_cli.h

SRC=main.c recv.c char.c db.c mail.c acfamily.c lock.c $(SQLSRC) $(RPCSRC)
OBJ=$(SRC:.c=.o)
COMMON_LIB=../common/libcommon.a

${TARGET}: $(RPCSRC) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(COMMON_LIB) $(SQL)
	$(RM) $(OBJ)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

rpc : ${RPCHTML} ${RPCGEN}
	$(PERL) ${RPCGEN} ${RPCHTML} -h . -o .
	rm -f ${RPCNOUSE}

