INCFLAGS=-I../include -I../../lua -I../../common -I../../common/utils
CFLAGS=-m64 -w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libchar.a
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
RANLIB=ranlib
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)
	$(RM) $(OBJ)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

