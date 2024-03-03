CC=clang
CFLAGS=-Wall -Wextra
OBJS=lexer.o
NONSTDLIBSRC=../lib/nonstdlib/src
LIB=$(NONSTDLIBSRC)/nonstdlib

all: nonstdlib lexer.o
	$(CC) $(OBJS) -I$(NONSTDLIBSRC) main.c -o kebab -L$(NONSTDLIBSRC) -lnonstdlib 

lexer.o:
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c lexer.c -o lexer.o

nonstdlib:
	$(MAKE) -C ../lib/nonstdlib/src
