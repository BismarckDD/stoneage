export CC=gcc
export RM=rm -f

INCFLAGS=-I../common -I./include
CFLAGS=-m64 -w -O3 -g -pipe -Wall -fcommon $(INCFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

ifeq ($(shell uname),CYGWIN_NT-5.1)
	SQLSRC=sasql_cygwin.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz -lpthread
	TARGET=../../csa/saac/saacjt
endif
ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/x86_64-linux-gnu/ -lmysqlclient -lz -lpthread
	TARGET=../../csa/saac/saacjt
endif

SRC=main.c saac_config.c tcp_struct.c char.c db.c mail.c acfamily.c lock.c $(SQLSRC)
OBJ=$(SRC:.c=.o)
COMMON_LIB=../common/libserverutil.a ../common/libutil.a
SERVER_LIB=../common/libsaacserver.a

${TARGET}: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(SERVER_LIB) $(COMMON_LIB) $(SQL)
	$(RM) $(OBJ)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

