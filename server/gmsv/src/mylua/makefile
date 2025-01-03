export CC=gcc -m32
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed


export SHELL=/bin/sh

INCFLAGS=-I. -I../../include -I../lua

CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

PROG=libmylua.a

SRC=mylua.c ablua.c init.c charbase.c npcbase.c function.c lssprotobase.c battlebase.c objectbase.c mapbase.c otherbase.c configbase.c itembase.c magicbase.c offlinebase.c netbase.c enemytempbase.c sasqlbase.c saacproto.c family.c petskill.c profession_skill.c

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
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 
	chmod 777 makefile
	
clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

