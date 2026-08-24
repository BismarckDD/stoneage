export CC=gcc
export RM=rm -f

INCFLAGS=-I../common -I./include
BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS=-m64 -O0 -g3 -ggdb -fno-omit-frame-pointer -fno-inline -pipe -Wall \
	-Wno-unused-result -fcommon $(INCFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
LDFLAGS=-rdynamic
else ifeq ($(BUILD),release)
CFLAGS=-m64 -O3 -DNDEBUG -w -pipe -Wall -fcommon $(INCFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
LDFLAGS=
else
$(error BUILD must be either debug or release)
endif
CFLAGS+=-MMD -MP

ifeq ($(shell uname),CYGWIN_NT-5.1)
	SQLSRC=sasql_cygwin.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz -lpthread
	TARGET=/home/franklin/csa/saac/saac
endif
ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/x86_64-linux-gnu/ -lmysqlclient -lz -lpthread
	TARGET=/home/franklin/csa/saac/saac
endif

SRC=main.c saac_config.c tcp_struct.c char.c db.c mail.c acfamily.c lock.c $(SQLSRC)
OBJ=$(SRC:.c=.o)
COMMON_LIB=../common/libserverutil.a ../common/libutil.a
SERVER_LIB=../common/libsaacserver.a

${TARGET}: $(OBJ) $(SERVER_LIB) $(COMMON_LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) \
		$(LDFLAGS) -Wl,--start-group $(SERVER_LIB) $(COMMON_LIB) -Wl,--end-group $(SQL)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *.d
	$(RM) *~

-include $(OBJ:.o=.d)
