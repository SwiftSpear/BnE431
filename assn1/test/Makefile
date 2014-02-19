all: test 

CC = gcc
CCFLAGS = -g -Wall -O3 -D_REENTRANT
LDFLAGS = -lm -pthread

test: test.o
	$(CC) $(LDFLAGS) -o test test.o

test.o: test.c
	$(CC) $(CCFLAGS) -c test.c -o test.o

clean:
	-rm *.o *~ test
