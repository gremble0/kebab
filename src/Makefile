CC=clang
CFLAGS=-Wall -Wextra -g -DDEBUG
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

NONSTDLIBSRC=../lib/nonstdlib/src
LEXERSRC=lexer
PARSERSRC=parser

OBJS=$(LEXERSRC)/*.o $(PARSERSRC)/*.o

all: lexer parser
	$(CC) $(CFLAGS) $(OBJS) -I$(LEXERSRC) -I$(PARSERSRC) -I$(NONSTDLIBSRC) main.c -o kebab $(LIBS)

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

nonstdlib:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	rm -f kebab

.PHONY: lexer parser
