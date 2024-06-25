CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LEXERSRC := ./lexer
PARSERSRC := ./parser
COMPILERSRC := ./compiler

OBJS := $(LEXERSRC)/*.o $(PARSERSRC)/*.o $(COMPILERSRC)/*.o

all: lexer parser compiler
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o kebab

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	$(MAKE) -C $(COMPILERSRC) clean

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

compiler:
	$(MAKE) -C $(COMPILERSRC)

.PHONY: lexer parser compiler
