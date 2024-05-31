NONSTDLIBROOT := ../lib/nonstdlib
NONSTDLIBSRC := $(NONSTDLIBROOT)/src
NONSTDLIBINCLUDE := $(NONSTDLIBROOT)/include
LEXERSRC := lexer
PARSERSRC := parser
RUNTIMESRC := runtime
UTILSSRC := utils

CC := clang
CFLAGS := -Wall -Wextra -g -O3
LIBS := -L$(NONSTDLIBSRC) -lnonstdlib

OBJS := $(LEXERSRC)/*.o $(PARSERSRC)/*.o $(RUNTIMESRC)/*.o $(UTILSSRC)/*.o

all: lexer parser runtime utils
	$(CC) $(CFLAGS) -I$(NONSTDLIBINCLUDE) -I. $(OBJS) main.c -o kebab $(LIBS)

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

runtime:
	$(MAKE) -C $(RUNTIMESRC)

utils:
	$(MAKE) -C $(UTILSSRC)

nonstdlib_debug:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC) debug

nonstdlib_release:
	$(MAKE) -C $(NONSTDLIBSRC) clean
	$(MAKE) -C $(NONSTDLIBSRC)

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	$(MAKE) -C $(RUNTIMESRC) clean
	$(MAKE) -C $(UTILSSRC) clean
	rm -f kebab

.PHONY: lexer parser runtime utils nonstdlib clean
