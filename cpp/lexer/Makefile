CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

OBJS := Lexer.o Token.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
