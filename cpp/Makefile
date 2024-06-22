CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LEXERSRC := ./lexer

OBJS := $(LEXERSRC)/*.o

all: lexer
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o kebab

clean:
	$(MAKE) -C $(LEXERSRC) clean

lexer:
	$(MAKE) -C $(LEXERSRC)

.PHONY: lexer
