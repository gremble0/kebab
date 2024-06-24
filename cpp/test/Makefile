CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LIBS := -L../../lib/googletest/build/lib -lgtest -lgtest_main
INCLUDES := -I../../lib/googletest/googletest/include/ -I..

LEXEROBJS := ../lexer/*.o
OBJS := main.o LexerTest.o Files.o $(LEXEROBJS)

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o run_tests $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
	rm -f run_tests
