export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I./include -I../lua -I../common -I../common/utils
CFLAGS=-m64 -w -O3 -Wall -pipe $(INCFLAGS)

ifeq ($(shell uname),Linux)
	SQL=-L/usr/lib/x86_64-linux-gnu/ -lmysqlclient -lz
	TARGET=../../csa/gmsv/gmsvjt
endif

COMMON_LIBS=../common/libsaacclient.a ../common/libgmsvserver.a ../common/libserverutil.a ../common/libutil.a
SUBDIR=char npc net map item battle magic mylua lua npc_lua
LIBS=net/libnet.a char/libchar.a npc/libnpc.a map/libmap.a item/libitem.a magic/libmagic.a battle/libbattle.a mylua/libmylua.a npc_lua/libnpc_lua.a ../lua/libluajit.a $(COMMON_LIBS)

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

$(TARGET): $(LIBS) $(OBJ) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS) $(LDFLAGS) $(SQL)

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

CUSTOM_DIR=char npc net map item battle magic mylua
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

