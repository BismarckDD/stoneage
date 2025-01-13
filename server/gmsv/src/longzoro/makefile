export CC=gcc -m32
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I. -I../../include -I.. -I../lua

CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz
endif

PROG=custom.a
SRC=move.c crypto.c characters.c luckstar.c newshop.c playerquestion.c $(SQLSRC)
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

