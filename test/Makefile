NONSTDLIBROOT := ../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ../src
LEXERSRC := $(KEBABSRC)/lexer
PARSERSRC := $(KEBABSRC)/parser

CC := clang
CFLAGS := -Wall -Wextra -g -O0
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

TESTOBJS := test_lexer.o test_parser.o utils.o
LEXEROBJS := $(LEXERSRC)/*.o
PARSEROBJS := $(PARSERSRC)/*.o

all: clean $(TESTOBJS)
	$(CC) $(CFLAGS) $(TESTOBJS) $(LEXEROBJS) $(PARSEROBJS) -o test_all test_all.c $(LIBS)

test_lexer.o:
	$(MAKE) -C $(LEXERSRC) clean all
	$(CC) $(CFLAGS) -I$(KEBABSRC) -I$(NONSTDLIBINCLUDE) $(LEXEROBJS) -c test_lexer.c -o test_lexer.o

test_parser.o:
	$(MAKE) -C $(PARSERSRC) clean all
	$(CC) $(CFLAGS) -I$(KEBABSRC) -I$(NONSTDLIBINCLUDE) $(PARSEROBJS) -c test_parser.c -o test_parser.o

utils.o:
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -c utils.c -o utils.o

clean:
	rm test_all *.o -f *.log

.PHONY: *.o
