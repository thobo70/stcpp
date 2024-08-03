CC = gcc
CFLAGS = -g -Wall -Wextra -Werror

all: hello c99test macro input

hello: hello.c
	$(CC) $(CFLAGS) -o hello hello.c

c99.l.c: c99.l
	flex -o $@ $<

c99test: c99.l.o
	$(CC) $(CFLAGS) -o c99test c99.l.o

macro: macrotest.o macro.o
	$(CC) $(CFLAGS) -o macro $^

input: input.o
	$(CC) $(CFLAGS) -o $@ $^

cmdline: expr.o cmdline.o input.o macro.o main.o
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -f hello c99.l.c c99test macro input cmdline *.o
