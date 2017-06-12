SHELL=/bin/sh

#
# clang seems to be better than gcc for this code?
#
CC ?= clang
OS=$(shell uname)

#
# We can do performance measurement only on x86_64 and x86
#
ARCH ?= $(shell uname -m)
supported_arch=x86_64 i686
test_arch=$(filter $(supported_arch), $(ARCH))

ifeq (,$(test_arch))
  $(error  "Unsupported architecture: $(ARCH).")
endif


ifeq (Darwin, $(findstring Darwin, ${OS}))
CPPFLAGS = -DDARWIN
else
CPPFLAGS = -DLINUX
endif


CPPFLAGS +=  -I../../include/ -I../../arch/${ARCH} -DNDEBUG
# CFLAGS += -std=gnu99 -Wall -O3 -pg 
CFLAGS += -std=gnu99 -Wall -O3 -g 

OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

TESTS = test_signing test_signings mod test_profiling

TEST_SRCS = $(addsuffix .c, ${TESTS})

all: ${TEST_SRCS}
	for test in ${TESTS} ; \
	  do ${CC} ${CFLAGS} $(CPPFLAGS) $$test.c ${OBJS} -o $$test; \
	done



check: all
	./test_signings


clean:
	rm -f ${TESTS}
	rm -rf *.dSYM
