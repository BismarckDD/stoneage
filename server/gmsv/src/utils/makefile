INCFLAGS=-I../../include -I../lua
CFLAGS=-w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libutil.a
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
  CC=gcc -m32
  RM=rm -f
  AR=ar cr
  RANLIB=ranlib
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

