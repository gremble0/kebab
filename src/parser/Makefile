CC=clang
CFLAGS=-Wall -Wextra -g -DDEBUG
LIBS=-L$(NONSTDLIBSRC) -lnonstdlib

NONSTDLIBSRC=../../lib/nonstdlib/src
LEXERSRC=../lexer
UTILSRC=../util

OBJS=atoms.o constructors.o factors.o parser.o parser_error.o primaries.o statements.o types.o utils.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -I$(LEXERSRC) -I$(UTILSRC) -c $< -o $@

clean:
	rm -f *.o

