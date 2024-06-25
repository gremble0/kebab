CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config

INCLUDES := -I.. -I$(LLVM_DIR)/llvm/include
# LDFLAGS := -L$(shell $(LLVM_CONFIG) --libdir) $(shell $(LLVM_CONFIG) --libs core) $(shell $(LLVM_CONFIG) --system-libs)
LDFLAGS := `$(LLVM_CONFIG) --cxxflags -ldflags --system-libs --libs core`

OBJS := Compiler.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
