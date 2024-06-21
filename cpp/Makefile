CC := clang++
CFLAGS := -Wall -Wextra -g -O3

LEXERSRC := ./lexer

all: lexer
	$(CC) $(CFLAGS) main.cpp -o kebab

lexer:
	$(MAKE) -C $(LEXERSRC)

.PHONY: lexer
