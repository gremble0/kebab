CC=clang
CFLAGS=-Wall -Wextra -g -O0
OBJS=test_lexer.o
SRC=../src
NONSTDLIBSRC=../lib/nonstdlib/src
LIBS=-L$(SRC) -lkebab -L$(NONSTDLIBSRC) -lnonstdlib

all: clean src $(OBJS)
	$(CC) $(CFLAGS) -o test_all $(OBJS) test_all.c $(LIBS)

test_lexer.o:
	$(CC) $(CFLAGS) -I$(SRC) -I$(NONSTDLIBSRC) -c test_lexer.c -o test_lexer.o

# %.o: %.c:
# 	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

src:
	$(MAKE) -C $(SRC) clean
	$(MAKE) -C $(SRC) lib

clean:
	rm test_all *.o -f
