INCFLAGS=-I. -I../../include -I../lua
FLTFLAGS="-s 1331200"   
export FLTFLAGS
CFLAGS=-w -Wall -pipe $(INCFLAGS)
PROG=libnpc_lua.a
SRC=npc_lua.c npc_lua_ex.c npc_lua_item.c npc_lua_nlg.c npc_lua_nl.c npc_lua_obj.c npc_lua_char.c npc_lua_battle.c npc_lua_game.c npc_lua_map.c npc_lua_spell.c npc_lua_sql.c

OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc -m32
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
	$(RM) $(OBJ)
	$(RM) *~

