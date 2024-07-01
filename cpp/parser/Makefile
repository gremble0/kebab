CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

INCLUDES := -I..

OBJS := AndTest.o Atom.o Comparison.o Constructor.o Expression.o Factor.o NotTest.o Parser.o Primary.o RootNode.o Statement.o Term.o Type.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LLVM_CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
