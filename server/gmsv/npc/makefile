INCFLAGS=-I../include -I../../lua -I../../common -I../../common/utils
BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS=-m64 -O0 -g3 -ggdb -fno-omit-frame-pointer -fno-inline -pipe -Wall \
	-Wno-unused-result -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else ifeq ($(BUILD),release)
CFLAGS=-m64 -w -O3 -Wall -pipe -fcommon -fgnu89-inline $(INCFLAGS) -MMD -MP
else
$(error BUILD must be either debug or release)
endif

TARGET=libnpc.a
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

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) *.o *.d
	$(RM) *~

-include $(OBJ:.o=.d)
