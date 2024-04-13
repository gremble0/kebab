NONSTDLIBSRC := ../lib/nonstdlib/src
SRC := ../src
LEXERSRC := $(SRC)/lexer
PARSERSRC := $(SRC)/parser

CC := clang
CFLAGS := -Wall -Wextra -g -O0
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

TESTOBJS := test_lexer.o test_parser.o
LEXEROBJS := $(LEXERSRC)/*.o
PARSEROBJS := $(PARSERSRC)/*.o

all: clean $(TESTOBJS)
	$(CC) $(CFLAGS) $(TESTOBJS) $(LEXEROBJS) -o test_all test_all.c $(LIBS)

test_lexer.o:
	$(MAKE) -C $(LEXERSRC) clean all
	$(CC) $(CFLAGS) -I$(LEXERSRC) -I$(NONSTDLIBSRC) $(LEXEROBJS) -c test_lexer.c -o test_lexer.o

test_parser.o:
	$(MAKE) -C $(PARSERSRC) clean all
	$(CC) $(CFLAGS) -I$(LEXERSRC) -I$(PARSERSRC) -I$(NONSTDLIBSRC) $(PARSEROBJS) -c test_parser.c -o test_parser.o

clean:
	rm test_all *.o -f

.PHONY: *.o
