CC=clang
CFLAGS=-Wall -Wextra -g -DDEBUG
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

NONSTDLIBSRC=../lib/nonstdlib/src
LEXERSRC=lexer
PARSERSRC=parser
UTILSRC=util

OBJS=$(LEXERSRC)/*.o $(PARSERSRC)/*.o $(UTILSRC)/*.o

all: lexer parser util
	$(CC) $(CFLAGS) $(OBJS) -I$(LEXERSRC) -I$(PARSERSRC) -I$(NONSTDLIBSRC) main.c -o kebab $(LIBS)

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

util:
	$(MAKE) -C $(UTILSRC)

nonstdlib:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	rm -f kebab

.PHONY: lexer parser util
