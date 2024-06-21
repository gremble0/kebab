CC := clang++
CFLAGS := -Wall -Wextra -g

LEXERSRC := ./lexer

all: lexer
	$(CC) $(CFLAGS) main.cpp -o kebab

lexer:
	$(MAKE) -C $(LEXERSRC)

.PHONY: lexer
