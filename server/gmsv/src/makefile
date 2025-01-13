export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I../include -I. -I./lua
CFLAGS=-m32 -w -O3 -Wall -pipe $(INCFLAGS)

ifeq ($(shell uname),Linux)
	SQL=-L/usr/lib/mysql/ -lmysqlclient -lz
	TARGET=../../../csa/gmsv/gmsvjt
endif

SUBDIR=char npc net map item battle magic longzoro mylua lua npc_lua utils
LIBS=net/libnet.a char/libchar.a npc/libnpc.a map/libmap.a item/libitem.a magic/libmagic.a battle/libbattle.a longzoro/custom.a mylua/libmylua.a npc_lua/libnpc_lua.a lua/libluajit.a utils/libutil.a

SRC=main.c init.c config_file.c buf.c msignal.c shop.c object.c handletime.c function.c log.c autil.c descrypt.c const.c

# SRC=$(wildcard *.c)
# convert *.c -> *.o, means OBJ is the counter part of SRC.
OBJ=$(SRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

LDFLAGS=-lm -lpthread -ldl

ifeq ($(shell uname),Linux)
	LDFLAGS+=
endif
ifeq ($(shell uname),SunOS)
	LDFLAGS+=-lnsl -lsocket
endif
ifeq ($(shell uname),FreeBSD)
	LDFLAGS+=
endif

all: $(TARGET)

$(TARGET): copyright $(LIBS) $(OBJ) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS) $(LDFLAGS) $(SQL)

copyright:
	$(SHELL) ./copyright.sh
	
clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

CUSTOM_DIR=char npc net map item battle magic longzoro mylua utils
clear:
	@for dir in $(CUSTOM_DIR); do\
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

data:	$(RPCSRC) $(ANIMDATAFILE)

$(LIBS):
	@target=`echo $@ | $(SED) s/\\\\/.\\*//`; \
	$(MAKE) -C $$target;

