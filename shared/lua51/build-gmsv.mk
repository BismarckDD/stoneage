# Build the shared Lua 5.1 runtime as a static library for GMSV (MinGW-w64).
#
# This file is separate from LuaJIT's own Makefile on purpose:
#   - GNU make searches GNUmakefile > makefile > Makefile, so adding either of
#     those names here would hijack LuaJIT's own build.
#   - GMSV only needs the plain Lua 5.1.4 sources, not the JIT core.
#     server/CMakeLists.txt and client/lua51.vcxproj use the same file list.
#
# Usage:
#   make -f build-gmsv.mk            # from this directory
#   make -f build-gmsv.mk -C ../../shared/lua51   # from elsewhere

CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
CFLAGS  ?= -m64 -O2 -w -fcommon
TARGET   = liblua51.a

# Plain Lua 5.1.4 sources only. Must stay in sync with:
#   server/CMakeLists.txt  -> LUA_SOURCES
#   client/lua51.vcxproj   -> ClCompile entries
SRC = \
	lapi.c lauxlib.c lbaselib.c lcode.c ldblib.c ldebug.c ldo.c ldump.c \
	lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c \
	lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c lstrlib.c \
	ltable.c ltablib.c ltm.c lundump.c lvm.c lzio.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) cr $@ $(OBJ)
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
