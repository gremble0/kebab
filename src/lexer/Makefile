NONSTDLIBROOT := ../../lib/nonstdlib
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ..

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG_LEXER -O3

OBJS := lexer.o token.o logging.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -I$(KEBABSRC) -c $< -o $@

clean:
	rm -f *.o
