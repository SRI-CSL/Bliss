ARCH ?= $(shell uname -m)

OS=$(shell uname)


ifneq ($(ARCH), x86_64)
  error  "Got me just one ARCH today."
endif


ifeq (Darwin, $(findstring Darwin, ${OS}))
LDFLAGS = -lmpfr -lgmp -L/usr/local/opt/openssl/lib -lcrypto
CFLAGS = -DDARWIN -I/usr/local/opt/openssl/include
else
LDFLAGS =  -lmpfr -lgmp -lcrypto
CFLAGS = -DLINUX
endif


CFLAGS= -Wall -I../../include/ -I../../arch/${ARCH} -ggdb


OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

LIBS=-lpthread  -lmpfr -lgmp ${LDFLAGS}

all:
	${CC} ${CFLAGS} speed.c ${OBJS} ${LIBS} -o  speed

check: all
	./speed


clean:
	rm -f speed
	rm -rf *.dSYM

