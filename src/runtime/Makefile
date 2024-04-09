NONSTDLIBSRC := ../../lib/nonstdlib/src
LEXERSRC := ../lexer
PARSERSRC := ../parser

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := eval.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -I$(LEXERSRC) -I$(PARSERSRC) -c $< -o $@

clean:
	rm -f *.o
