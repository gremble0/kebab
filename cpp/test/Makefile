CC := clang++
CFLAGS := -Wall -Wextra -g -std=c++17

LIBS := -L../../lib/googletest/build/lib -lgtest
INCLUDES := ../../lib/googletest/googletest/include/

OBJS := main.o

all: $(OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDES) main.cpp -o test $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
	rm test
