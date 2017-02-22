SHELL=/bin/sh

CC=clang
OS=$(shell uname)

ARCH ?= $(shell uname -m)


ifneq ($(ARCH), x86_64)
  error  "Got me just one ARCH today."
endif


LIBRARYNAME=libbliss

ifeq (Darwin, $(findstring Darwin, ${OS}))
LIBRARY = ${LIBRARYNAME}.dylib
LIBFLAGS = -dynamiclib
LDFLAGS = 
CPPFLAGS = -DDARWIN
else
LIBRARY = ${LIBRARYNAME}.so              
LIBFLAGS = -shared -Wl,-soname,${LIBRARY}
LDFLAGS =
CPPFLAGS = -DLINUX
endif

# CPPFLAGS += -I./include  -I./arch/${ARCH} -DNDEBUG
# CFLAGS += -std=c99 -fPIC -Wall -O3
CPPFLAGS += -I./include  -I./arch/${ARCH}
CFLAGS += -std=c99 -fPIC -Wall -g

SRC_GLOBS = $(addsuffix /*.c,src)
SRC = $(sort $(wildcard $(SRC_GLOBS)))

OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

TARGET = lib/${LIBRARY}


all: ${TARGET}
	@echo Done.	


$(TARGET): $(OBJ) | lib
	$(CC) $(LIBFLAGS) $(OBJ) $(LDFLAGS) -o $@  

obj:
	mkdir -p obj

obj/%.o: src/%.c | obj
	${CC} $(CPPFLAGS) ${CFLAGS} $< -c -o $@

lib:
	mkdir -p lib


UNIT_TESTS=./tests/static

TOOLS = ./tools

check: $(TARGET)
	make -C ${UNIT_TESTS} check

tools: $(TARGET)
	make -C ${TOOLS} 


clean:
	rm -f *~ *.o
	rm -rf obj lib
	make -C ${UNIT_TESTS} clean
	make -C ${TOOLS} clean
	make -C ./tools clean


.PHONY: clean tools

