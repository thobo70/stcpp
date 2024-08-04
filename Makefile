CC = gcc
BINDIR = ./bin
SRCDIR = ./src
OBJS = $(BINDIR)/expr.o $(BINDIR)/cmdline.o $(BINDIR)/input.o $(BINDIR)/macro.o $(BINDIR)/main.o
TARGET = $(BINDIR)/stcpp
CFLAGS = -g -Wall -Wextra -Werror

all: $(BINDIR) $(TARGET)


$(BINDIR): 
	mkdir -p bin

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -f $(BINDIR)
