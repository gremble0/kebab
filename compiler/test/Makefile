CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

LIBS := -L../../lib/googletest/build/lib -lgtest -lgtest_main
INCLUDES := -I../../lib/googletest/googletest/include/ -I..

LEXEROBJS := ../lexer/*.o
PARSEROBJS := ../parser/*.o
COMPILEROBJS := ../compiler/*.o
LOGGINGOBJS := ../logging/*.o
SRCOBJS := $(LEXEROBJS) $(PARSEROBJS) $(COMPILEROBJS) $(LOGGINGOBJS)
TESTOBJS := main.o LexerTest.o ParserTest.o CompilerTest.o Files.o

LLVM_DIR := ../../lib/llvm-project
LLVM_CONFIG := $(LLVM_DIR)/build/bin/llvm-config
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --system-libs --libs core)

all: $(TESTOBJS)
	$(MAKE) -C ..
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDES) $(TESTOBJS) $(SRCOBJS) -o run_tests $(LIBS) $(LLVM_LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C .. clean
	rm -f *.o
	rm -f run_tests
