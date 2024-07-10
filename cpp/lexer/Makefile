CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

OBJS := Lexer.o Token.o

INCLUDES := -I..

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
