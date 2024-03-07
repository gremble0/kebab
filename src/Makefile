CC=clang
CFLAGS=-Wall -Wextra -g
OBJS=main.o parser.o lexer.o token.o
NONSTDLIBSRC=../lib/nonstdlib/src
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

all: clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o kebab $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -c $< -o $@

nonstdlib:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	rm -f *.o
	rm -f kebab
