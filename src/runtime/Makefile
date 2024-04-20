NONSTDLIBSRC := ../../lib/nonstdlib/src
KEBABSRC := ..

CC := clang
CFLAGS := -Wall -Wextra -g -DDEBUG
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := eval.o

all: clean $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -I$(KEBABSRC) -c $< -o $@

clean:
	rm -f *.o
