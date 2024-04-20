NONSTDLIBSRC := ../lib/nonstdlib/src
LEXERSRC := lexer
PARSERSRC := parser
RUNTIMESRC := runtime

CC := clang
CFLAGS := -Wall -Wextra -g
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := $(LEXERSRC)/*.o $(PARSERSRC)/*.o $(RUNTIMESRC)/*.o

all: lexer parser runtime
	$(CC) $(CFLAGS) -I$(NONSTDLIBSRC) -I. $(OBJS) main.c -o kebab $(LIBS)

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

runtime:
	$(MAKE) -C $(RUNTIMESRC)

nonstdlib:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	$(MAKE) -C $(RUNTIMESRC) clean
	rm -f kebab

.PHONY: lexer parser runtime nonstdlib clean
