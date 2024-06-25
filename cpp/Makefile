CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LEXERSRC := ./lexer
PARSERSRC := ./parser
COMPILERSRC := ./compiler

LLVM_DIR := ../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --system-libs --libs core)

INCLUDES := -I.

OBJS := $(LEXERSRC)/*.o $(PARSERSRC)/*.o $(COMPILERSRC)/*.o

all: lexer parser compiler
	$(CC) $(CFLAGS) $(LLVM_CXXFLAGS) $(INCLUDES) $(OBJS) main.cpp -o kebab $(LLVM_LDFLAGS)

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
