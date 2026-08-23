export CC=gcc
export RM=rm -f
export AR=ar cr
export RANLIB=ranlib
export SHELL=/bin/sh

INCFLAGS=-I. -I./utils
CFLAGS=-m64 -w -O3 -Wall -pipe $(INCFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
SAAC_CFLAGS=$(CFLAGS) -I../saac/include
GMSV_CFLAGS=$(CFLAGS) -I../gmsv/include -I../lua

LIBUTIL=libutil.a
LIBSERVERUTIL=libserverutil.a
LIBSAACSERVER=libsaacserver.a
LIBSAACCLIENT=libsaacclient.a
LIBGMSVSERVER=libgmsvserver.a
TARGET=$(LIBUTIL) $(LIBSERVERUTIL) $(LIBSAACSERVER) $(LIBSAACCLIENT) $(LIBGMSVSERVER)
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

UTILS_SRC=utils/md5.c utils/util_string.c utils/util_file.c utils/util_time.c
UTILS_OBJ=$(UTILS_SRC:.c=.o)
SERVERUTIL_SRC=server_util.c workspace.c
SERVERUTIL_OBJ=$(SERVERUTIL_SRC:.c=.o)
SAACSERVER_SRC=saac_server_send.c saac_server_recv.c
SAACSERVER_OBJ=$(SAACSERVER_SRC:.c=.o)
SAACCLIENT_SRC=saac_client_send.c saac_client_recv.c
SAACCLIENT_OBJ=$(SAACCLIENT_SRC:.c=.o)
GMSVSERVER_SRC=gmsv_server_send.c gmsv_server_recv.c
GMSVSERVER_OBJ=$(GMSVSERVER_SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
RANLIB=ranlib
SHELL=/bin/sh
endif

all: copyright $(TARGET)

${LIBUTIL}: $(UTILS_OBJ)
	$(RM) $(LIBUTIL)
	$(AR) $(LIBUTIL) $(UTILS_OBJ)
	$(RANLIB) $(LIBUTIL)
	$(RM) $(UTILS_OBJ)

$(UTILS_OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

${LIBSERVERUTIL}: $(SERVERUTIL_OBJ)
	$(RM) $(LIBSERVERUTIL)
	$(AR) $(LIBSERVERUTIL) $(SERVERUTIL_OBJ)
	$(RANLIB) $(LIBSERVERUTIL)
	$(RM) $(SERVERUTIL_OBJ)

$(SERVERUTIL_OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

${LIBSAACSERVER}: $(SAACSERVER_OBJ)
	$(RM) $(LIBSAACSERVER)
	$(AR) $(LIBSAACSERVER) $(SAACSERVER_OBJ)
	$(RANLIB) $(LIBSAACSERVER)
	$(RM) $(SAACSERVER_OBJ)

$(SAACSERVER_OBJ): %.o: %.c
	$(CC) $(SAAC_CFLAGS) -c $< -o $@

${LIBSAACCLIENT}: $(SAACCLIENT_OBJ)
	$(RM) $(LIBSAACCLIENT)
	$(AR) $(LIBSAACCLIENT) $(SAACCLIENT_OBJ)
	$(RANLIB) $(LIBSAACCLIENT)
	$(RM) $(SAACCLIENT_OBJ)

$(SAACCLIENT_OBJ): %.o: %.c
	$(CC) $(GMSV_CFLAGS) -c $< -o $@

${LIBGMSVSERVER}: $(GMSVSERVER_OBJ)
	$(RM) $(LIBGMSVSERVER)
	$(AR) $(LIBGMSVSERVER) $(GMSVSERVER_OBJ)
	$(RANLIB) $(LIBGMSVSERVER)
	$(RM) $(GMSVSERVER_OBJ)

$(GMSVSERVER_OBJ): %.o: %.c
	$(CC) $(GMSV_CFLAGS) -c $< -o $@

copyright:
	$(SHELL) ./copyright.sh

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~
