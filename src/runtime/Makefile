NONSTDLIBROOT := ../../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ..

CC := clang
CFLAGS := -Wall -Wextra -g
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := runtime.o statements.o expressions.o constructors.o scope.o error.o operators.o primaries.o atoms.o factors.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -I$(KEBABSRC) -c $< -o $@

clean:
	rm -f *.o
