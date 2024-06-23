CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LIBS := -L../../lib/googletest/build/lib -lgtest -lgtest_main
INCLUDES := -I../../lib/googletest/googletest/include/ -I..

OBJS := main.o LexerTest.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o run_tests $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
	rm -f run_tests
