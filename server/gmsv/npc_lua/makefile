INCFLAGS=-I../.. -I../include -I../../lua -I../../common
FLTFLAGS="-s 1331200"   
export FLTFLAGS
BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS=-m64 -O0 -g3 -ggdb -fno-omit-frame-pointer -fno-inline -pipe -Wall \
	-Wno-unused-result -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else ifeq ($(BUILD),release)
CFLAGS=-m64 -w -Wall -pipe -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else
$(error BUILD must be either debug or release)
endif
PROG=libnpc_lua.a
SRC=npc_lua.c npc_lua_ex.c npc_lua_item.c npc_lua_nlg.c npc_lua_nl.c npc_lua_obj.c npc_lua_char.c npc_lua_battle.c npc_lua_game.c npc_lua_map.c npc_lua_spell.c npc_lua_sql.c

OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
	CC=gcc
	RM=rm -f
	AR=ar cr
	MV=mv
	RANLIB=ranlib
	SED=sed
	SHELL=/bin/sh
endif

all: $(PROG)
$(PROG): $(OBJ)
	$(RM) $(PROG)
	$(AR) $(PROG) $(OBJ)
	$(RANLIB) $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
depend:
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> Makefile 

clean:
	$(RM) $(PROG)
	$(RM) *.o *.d
	$(RM) *~

-include $(OBJ:.o=.d)
