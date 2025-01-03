INCFLAGS=-I../../include -I../lua
CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

PROG=libchar.a
SRC=char_base.c char.c char_event.c char_data.c skill.c title.c\
addressbook.c chatmagic.c event.c char_walk.c encount.c pet.c \
enemy.c pet_event.c char_talk.c char_party.c char_item.c \
chatroom.c petmail.c trade.c family.c char_angel.c

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

depend:
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile 
	chmod 777 makefile

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

