NONSTDLIBSRC := ../../lib/nonstdlib/src
LEXERSRC := ../lexer

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG_PARSER
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := atoms.o constructors.o expressions.o factors.o parser.o parser_error.o primaries.o statements.o types.o utils.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -I$(LEXERSRC) -c $< -o $@

clean:
	rm -f *.o
