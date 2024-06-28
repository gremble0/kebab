CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

INCLUDES := -I..

OBJS := AndTest.o Atom.o Comparison.o Constructor.o Expression.o Factor.o NotTest.o Parser.o Primary.o RootNode.o Statement.o Term.o Type.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
