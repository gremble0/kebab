NONSTDLIBROOT := ../../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ..

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG_PARSER
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := atoms.o constructors.o expressions.o factors.o parser.o error.o primaries.o statements.o types.o utils.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -I$(KEBABSRC) -c $< -o $@

clean:
	rm -f *.o
