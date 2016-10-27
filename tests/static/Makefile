ARCH ?= $(shell uname -m)


ifneq ($(ARCH), x86_64)
  error  "Got me just one ARCH today."
endif


CFLAGS= -Wall -I../../include/ -I../../arch/${ARCH} -ggdb


OBJDIR=../../obj
OBJ_GLOBS = $(addsuffix /*.o,${OBJDIR})
OBJS = $(sort $(wildcard ${OBJ_GLOBS}))

LIBS=-lpthread

all:
	${CC} ${CFLAGS} speed.c ${OBJS} ${LIBS} -o  speed

check: all
	./speed


clean:
	rm -f speed
	rm -rf *.dSYM

