ARCH ?= $(shell uname -m)

OS=$(shell uname)


ifneq ($(ARCH), x86_64)
  error  "Got me just one ARCH today."
endif


ifeq (Darwin, $(findstring Darwin, ${OS}))
LDFLAGS = -lmpfr -lgmp -L/usr/local/opt/openssl/lib -lcrypto  -L../../lib/libbliss.dylib
CFLAGS = -DDARWIN -I/usr/local/opt/openssl/include
else
LDFLAGS =  -lmpfr -lgmp -lcrypto  -L../../lib/libbliss.so
CFLAGS = -DLINUX
endif


CFLAGS += -Wall -I../../include/ -I../../arch/${ARCH} -ggdb


OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

LIBS +=-lpthread  -lmpfr -lgmp ${LDFLAGS}


TESTS = speed  sample
TEST_SRCS = $(addsuffix .c, ${TESTS})

all: ${TEST_SRCS}
	for test in ${TESTS} ; do ${CC} ${CFLAGS} $$test.c ${OBJS} ${LIBS} -o $$test ; done

check: all
	for test in ${TESTS} ; do ./$$test; done


clean:
	rm -f ${TESTS}
	rm -rf *.dSYM

