CC = gcc
BINDIR = ./bin
SRCDIR = ./src
OBJS = $(BINDIR)/expr.o $(BINDIR)/cmdline.o $(BINDIR)/input.o $(BINDIR)/macro.o $(BINDIR)/main.o
TARGET = $(BINDIR)/stcpp
CFLAGS = -g -Wall -Werror -Wextra -pedantic -Isrc


.PHONY: all clean test target

all: target

target: $(BINDIR) $(TARGET)

$(BINDIR): 
	mkdir -p bin

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -rf $(BINDIR)

test: target
	./$(TARGET) -I/usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux -I src src/main.c -
