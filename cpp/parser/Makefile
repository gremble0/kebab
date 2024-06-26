CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

INCLUDES := -I..

OBJS := Statement.o RootNode.o Expression.o

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
