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
# CFLAGS += -std=c99 -fPIC -Wall -O3 -pg

CPPFLAGS += -I./include  -I./arch/${ARCH} -DNDEBUG
CFLAGS += -std=c99 -fPIC -Wall -O3 -g -Wsign-conversion -funroll-loops

SRC_GLOBS = $(addsuffix /*.c,src)
SRC = $(sort $(wildcard $(SRC_GLOBS)))

OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

TARGET = lib/${LIBRARY}


all: ${TARGET} tests
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

check: $(TARGET) tests
	make -C ${UNIT_TESTS} check

tests: $(TARGET)
	make -C ${UNIT_TESTS}

tools: $(TARGET)
	make -C ${TOOLS} 


clean:
	rm -f *~ *.o
	rm -rf obj lib
	make -C ${UNIT_TESTS} clean
	make -C ${TOOLS} clean
	make -C ./tools clean


.PHONY: clean tools tests

