CC=clang
CFLAGS=-Wall -Wextra -g -DDEBUG
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

NONSTDLIBSRC=../../lib/nonstdlib/src

OBJS=lexer.o token.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c $< -o $@

clean:
	rm -f *.o
