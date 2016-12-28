SHELL=/bin/sh

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
CFLAGS += -std=gnu99 -Wall -O3


OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

TESTS = speed sample test_keys test_bernoulli test_pos_binary test_pos_binary2 \
        test_gaussian test_gaussian2  test_poly test_ntt test_blzzd test_blzzd1024

TEST_SRCS = $(addsuffix .c, ${TESTS})

all: ${TEST_SRCS}
	for test in ${TESTS} ; \
	  do ${CC} ${CFLAGS} $(CPPFLAGS) $$test.c ${OBJS} -o $$test; \
	done



check: all
	for test in ${TESTS} ; do ./$$test; done


clean:
	rm -f ${TESTS}
	rm -rf *.dSYM
