CC := clang++
CFLAGS := -Wall -Wextra -g

OBJS := Lexer.o Token.o

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
