CC := clang++
CFLAGS := -Wall -Wextra -g

LEXERSRC := ./lexer

OBJS := $(LEXERSRC)/*.o

all: lexer
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o kebab

lexer:
	$(MAKE) -C $(LEXERSRC)

.PHONY: lexer
