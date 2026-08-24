export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I./include -I../lua -I../common -I../common/utils
BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS=-m64 -O0 -g3 -ggdb -fno-omit-frame-pointer -fno-inline -pipe -Wall \
	-Wno-unused-result -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else ifeq ($(BUILD),release)
CFLAGS=-m64 -w -O3 -Wall -pipe -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else
$(error BUILD must be either debug or release)
endif

ifeq ($(shell uname),Linux)
	SQL=-L/usr/lib/x86_64-linux-gnu/ -lmysqlclient -lz
	TARGET=/home/franklin/csa/gmsv/gmsv
endif

COMMON_LIBS=../common/libsaacclient.a ../common/libgmsvserver.a ../common/libserverutil.a ../common/libutil.a
SUBDIR=char npc net map item battle magic mylua npc_lua
SUBDIR_LIBS=$(foreach dir,$(SUBDIR),$(dir)/lib$(dir).a)
PREBUILT_LIBS=../lua/libluajit.a
LIBS=$(SUBDIR_LIBS) $(PREBUILT_LIBS) $(COMMON_LIBS)

SRC=main.c init.c config_file.c buf.c msignal.c shop.c object.c handletime.c function.c log.c autil.c descrypt.c legacy_util.c optional_features.c const.c

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
ifeq ($(BUILD),debug)
LDFLAGS+=-rdynamic
endif

all: $(TARGET)

$(TARGET): $(LIBS) $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) \
		-Wl,--start-group $(LIBS) -Wl,--end-group $(LDFLAGS) $(SQL)

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *.d
	$(RM) *~

CUSTOM_DIR=char npc net map item battle magic mylua
clear:
	@for dir in $(CUSTOM_DIR); do\
		$(MAKE) -C $$dir clean;\
	done
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *.d
	$(RM) *~

data:	$(RPCSRC) $(ANIMDATAFILE)

# Always descend into subdirectories so source changes are never missed.
# (A plain "$(LIBS):" rule is skipped whenever the .a already exists.)
$(SUBDIR_LIBS): FORCE
	@target=`echo $@ | $(SED) 's,/.*,,'`; \
	$(MAKE) -C $$target;

.PHONY: FORCE
FORCE:

-include $(OBJ:.o=.d)
