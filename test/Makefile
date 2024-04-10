NONSTDLIBSRC := ../lib/nonstdlib/src
SRC := ../src
LEXERSRC := $(SRC)/lexer
PARSERSRC := $(SRC)/parser

CC := clang
CFLAGS := -Wall -Wextra -g -O0
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

TESTOBJS := test_lexer.o
LEXEROBJS := $(LEXERSRC)/*.o
PARSEROBJS := $(PARSEROBJS)/*.o

all: clean $(TESTOBJS)
	$(CC) $(CFLAGS) $(TESTOBJS) $(LEXEROBJS) -o test_all test_all.c $(LIBS)

test_lexer.o:
	$(MAKE) -C $(LEXERSRC) clean all
	$(CC) $(CFLAGS) -I$(LEXERSRC) -I$(NONSTDLIBSRC) $(LEXEROBJS) -c test_lexer.c -o test_lexer.o

clean:
	rm test_all *.o -f

.PHONY: *.o
