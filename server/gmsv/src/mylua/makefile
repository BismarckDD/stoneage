export CC=gcc -m32
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed

INCFLAGS=-I. -I../../include -I../lua

CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libmylua.a

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

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

