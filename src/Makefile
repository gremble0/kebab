CC=clang
CFLAGS=-Wall -Wextra
OBJS=lexer.o
LIB=../lib/nonstdlib/src/libnonstdlib.a

all: nonstdlib lexer.o
	$(CC) $(OBJS) main.c -o kebab

lexer.o:
	$(CC) $(CFLAGS) -c lexer.c -o lexer.o

nonstdlib:
	$(MAKE) -C ../lib/nonstdlib/src
