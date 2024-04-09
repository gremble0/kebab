NONSTDLIBSRC=../../lib/nonstdlib/src

CC=clang
CFLAGS=-Wall -Wextra -g -DDEBUG
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

OBJS=lexer.o token.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c $< -o $@

clean:
	rm -f *.o
