CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

LEXERSRC := ./lexer
PARSERSRC := ./parser
COMPILERSRC := ./compiler
LOGGINGSRC := ./logging

LLVM_DIR := ../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --system-libs --libs core)

INCLUDES := -I.

OBJS := $(LEXERSRC)/*.o $(PARSERSRC)/*.o $(COMPILERSRC)/*.o $(LOGGINGSRC)/*.o

all: lexer parser compiler logging
	$(CC) $(LLVM_CFLAGS) $(CFLAGS) $(INCLUDES) $(OBJS) main.cpp -o kebab $(LLVM_LDFLAGS)

clean:
	$(MAKE) -C $(LEXERSRC) clean
	$(MAKE) -C $(PARSERSRC) clean
	$(MAKE) -C $(COMPILERSRC) clean
	$(MAKE) -C $(LOGGINGSRC) clean

lexer:
	$(MAKE) -C $(LEXERSRC)

parser:
	$(MAKE) -C $(PARSERSRC)

compiler:
	$(MAKE) -C $(COMPILERSRC)

logging:
	$(MAKE) -C $(LOGGINGSRC)

.PHONY: lexer parser compiler logging
