INCFLAGS=-I../../include -I../lua
CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libnpc.a
SRC=$(wildcard *.c)
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

depend:
	$(MV) makefile makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' makefile.bak>makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> makefile
	chmod 777 makefile
	
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

