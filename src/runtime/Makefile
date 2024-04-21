NONSTDLIBROOT := ../../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ..

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := eval.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -I$(KEBABSRC) -c $< -o $@

clean:
	rm -f *.o
