NONSTDLIBROOT := ../../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
KEBABSRC := ..
UTILSSRC := $(KEBABSRC)/utils
LEXERSRC := $(KEBABSRC)/lexer
PARSERSRC := $(KEBABSRC)/parser

CC := clang
CFLAGS := -Wall -Wextra -g -O0
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

TESTOBJS := test_lexer.o test_parser.o utils.o
LEXEROBJS := $(LEXERSRC)/*.o
PARSEROBJS := $(PARSERSRC)/*.o
UTILSOBJS := $(UTILSSRC)/*.o

all: clean $(TESTOBJS)
	$(CC) $(CFLAGS) $(TESTOBJS) $(LEXEROBJS) $(PARSEROBJS) $(UTILSOBJS) -o test_all test_all.c $(LIBS)

test_lexer.o:
	$(MAKE) -C $(LEXERSRC) clean all
	$(CC) $(CFLAGS) -I$(KEBABSRC) -I$(NONSTDLIBINCLUDE) -c test_lexer.c -o test_lexer.o

test_parser.o:
	$(MAKE) -C $(PARSERSRC) clean all
	$(CC) $(CFLAGS) -I$(KEBABSRC) -I$(NONSTDLIBINCLUDE) -c test_parser.c -o test_parser.o

utils.o:
	$(MAKE) -C $(UTILSSRC) clean all
	$(CC) $(CFLAGS) -I$(UTILSSRC) -I$(NONSTDLIBINCLUDE) -c utils.c -o utils.o

clean:
	rm test_all *.o -f *.log

.PHONY: *.o
