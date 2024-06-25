CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)

INCLUDES := -I..

OBJS := Compiler.o

# Targets
all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LLVM_CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o
