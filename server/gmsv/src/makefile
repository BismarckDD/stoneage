export CC=gcc -m32
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh
export PERL=/usr/bin/env perl

INCFLAGS=-I../include -I./lua

CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

ifeq ($(shell uname),Linux)
	SQL=-L/usr/lib/mysql/ -lmysqlclient -lz
	TARGET=../../../csa/gmsv/gmsvjt
endif

RPCGEN=~/bin/lsgen.perl
CLIRPCHTML=~/doc/saacproto.html
CLIRPCSRC=saacproto_cli.c saacproto_util.c
CLIRPCINCLUDE=include/saacproto_cli.h include/saacproto_util.h
CLIRPCNOUSE=saacproto_serv.c include/saacproto_serv.h
CLIRPCOPTION=-h include -o . -euc 

SERVRPCHTML=~/doc/lssproto.html
SERVRPCSRC=lssproto_serv.c lssproto_util.c
SERVRPCINCLUDE=include/lssproto_serv.h include/lssproto_util.h
SERVRPCNOUSE=lssproto_cli.c include/lssproto_cli.h
SERVRPCOPTION= -h include -o . -euc -e

RPCSRC=$(CLIRPCSRC) $(SERVRPCSRC)

# CHAR_ANIM
ANIMDATAGEN=~/bin/convertStringtographicnumber.perl
ANIMDATATXT=include/anim_tbl.h
ANIMDATAFILE=char/ls2data.h


SUBDIR=char npc net map item battle magic longzoro mylua lua npc_lua utils
LIBS=char/libchar.a npc/libnpc.a net/libnet.a map/libmap.a item/libitem.a magic/libmagic.a battle/libbattle.a longzoro/custom.a mylua/libmylua.a npc_lua/libnpc_lua.a lua/libluajit.a utils/libutil.a

SRC=main.c init.c config_file.c buf.c msignal.c md5.c shop.c callfromac.c callfromcli.c object.c handletime.c function.c log.c autil.c descrypt.c const.c $(CLIRPCSRC) $(SERVRPCSRC)

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

$(TARGET): copyright $(RPCSRC) $(ANIMDATAFILE) $(LIBS) $(OBJ) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS) $(LDFLAGS) $(SQL)

copyright:
	$(SHELL) ./copyright.sh
	
depend: $(RPCSRC) $(ANIMDATAFILE)
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir depend;\
	done
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 
	chmod 777 makefile

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

data:	$(RPCSRC) $(ANIMDATAFILE)

$(CLIRPCSRC): #$(RPCGEN) $(CLIRPCHTML) $(CLIRPCGEN)

$(SERVRPCSRC): #$(RPCGEN) $(SERVRPCHTML) $(SERVRPCGEN)

$(ANIMDATAFILE): #$(ANIMDATAGEN) $(ANIMDATATXT)

$(LIBS):
	@target=`echo $@ | $(SED) s/\\\\/.\\*//`; \
	$(MAKE) -C $$target;

