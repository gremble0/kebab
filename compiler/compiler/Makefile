CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)

INCLUDES := -I..

OBJS := Compiler.o Scope.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(LLVM_CFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
