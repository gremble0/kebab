CC=clang
CFLAGS=-Wall -Wextra
OBJS=lexer.o main.o
NONSTDLIBSRC=../lib/nonstdlib/src
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

all: clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o kebab $(LIBS)

main.o:
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c main.c -o main.o

lexer.o:
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c lexer.c -o lexer.o

nonstdlib:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	rm -f *.o
	rm -f kebab
