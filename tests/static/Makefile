SHELL=/bin/sh

CC=clang
ARCH ?= $(shell uname -m)

OS=$(shell uname)


ifneq ($(ARCH), x86_64)
  error  "Got me just one ARCH today."
endif


ifeq (Darwin, $(findstring Darwin, ${OS}))
CPPFLAGS = -DDARWIN
else
CPPFLAGS = -DLINUX
endif


CPPFLAGS +=  -I../../include/ -I../../arch/${ARCH} 
# CFLAGS += -std=gnu99 -Wall -O3
CFLAGS += -std=gnu99 -Wall -g

OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

TESTS = test_signing test_signings 

TEST_SRCS = $(addsuffix .c, ${TESTS})

all: ${TEST_SRCS}
	for test in ${TESTS} ; \
	  do ${CC} ${CFLAGS} $(CPPFLAGS) $$test.c ${OBJS} -o $$test; \
	done



check: all
	for count in 0 1 2 3 4 ;  do ./test_signing $$count 100 ; done


clean:
	rm -f ${TESTS}
	rm -rf *.dSYM
