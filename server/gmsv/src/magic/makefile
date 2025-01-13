INCFLAGS=-I../../include -I.. -I../lua
CFLAGS=-m32 -w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libmagic.a
SRC=magic.c magic_base.c magic_field.c
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
	CC=gcc
	RM=rm -f
	AR=ar cr
	MV=mv
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

