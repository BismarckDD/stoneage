export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I.
CFLAGS=-m32 -w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libcommon.a
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
RANLIB=ranlib
endif

all: $(TARGET)

${TARGET}: $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)
	$(RM) $(OBJ)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

