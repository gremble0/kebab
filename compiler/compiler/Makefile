CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

INCLUDES := -I..

OBJS := Compiler.o Scope.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LLVM_CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
