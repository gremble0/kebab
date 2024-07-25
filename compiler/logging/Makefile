CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++20

OBJS := Logger.o

INCLUDES := -I..

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
