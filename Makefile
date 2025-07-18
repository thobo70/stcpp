CC = gcc
BINDIR = ./bin
SRCDIR = ./src
OBJS = $(BINDIR)/exprint.o $(BINDIR)/cmdline.o $(BINDIR)/input.o $(BINDIR)/macro.o $(BINDIR)/main.o
TARGET = $(BINDIR)/stcpp
CFLAGS = -g -Og -Wall -Werror -Wextra -pedantic -Isrc
# CFLAGS = -DNDEBUG -Oz -Wall -Werror -Wextra -pedantic -Isrc

.PHONY: all clean test test2 test-all test-basic test-recursive test-edge test-conditionals test-undef test-include test-token-pasting test-stringification doc lint

all: target

target: $(BINDIR) $(TARGET)

$(BINDIR): 
	mkdir -p bin

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -rf $(BINDIR) test.out test_results doc

# Original test (for backward compatibility)
test: target
	./$(TARGET) -Itest -D "__STDC__ 1" -D "__STDC_VERSION__ 1" test/test.c test.out

# Comprehensive test suite
test-all: $(TARGET)
	@echo "Running comprehensive test suite..."
	@./test/run_tests.sh

# Individual test targets
test-basic: target test_results
	@echo "Testing basic macro functionality..."
	./$(TARGET) -Itest test/test_basic.c test_results/test_basic.out
	@echo "Output:"
	@head -20 test_results/test_basic.out

test-recursive: target test_results
	@echo "Testing recursive macro expansion..."
	./$(TARGET) -Itest test/test_recursive.c test_results/test_recursive.out
	@echo "Output:"
	@head -20 test_results/test_recursive.out

test-edge: target test_results
	@echo "Testing edge cases..."
	./$(TARGET) -Itest test/test_edge_cases.c test_results/test_edge_cases.out
	@echo "Output:"
	@head -20 test_results/test_edge_cases.out

test-conditionals: target test_results
	@echo "Testing conditional compilation..."
	./$(TARGET) -Itest test/test_conditionals.c test_results/test_conditionals.out
	@echo "Output:"
	@head -20 test_results/test_conditionals.out

test-undef: target test_results
	@echo "Testing #undef functionality..."
	./$(TARGET) -Itest test/test_undef.c test_results/test_undef.out
	@echo "Output:"
	@head -20 test_results/test_undef.out

test-include: target test_results
	@echo "Testing file inclusion..."
	./$(TARGET) -Itest test/test_include.c test_results/test_include.out
	@echo "Output:"
	@head -20 test_results/test_include.out

test-token-pasting: target test_results
	@echo "Testing token pasting (##) operator..."
	./$(TARGET) -Itest test/test_token_pasting.c test_results/test_token_pasting.out
	@echo "Output:"
	@head -20 test_results/test_token_pasting.out

test-stringification: target test_results
	@echo "Testing stringification (#) operator..."
	./$(TARGET) -Itest test/test_stringification.c test_results/test_stringification.out
	@echo "Output:"
	@head -20 test_results/test_stringification.out

# Create test results directory
test_results:
	mkdir -p test_results

# Static analysis and linting
lint:
	@echo "Running cpplint on source files..."
	@if command -v cpplint >/dev/null 2>&1; then \
		echo "Running cpplint..."; \
		cpplint --filter=-whitespace/tab,-build/include_subdir $(SRCDIR)/*.c $(SRCDIR)/*.h 2>&1 | grep -v "Done processing" || true; \
		echo ""; \
		echo "Use 'cpplint --help' for filtering options if needed."; \
	else \
		echo "cpplint not found. Install with: pip install cpplint"; \
	fi
	@echo "Lint check complete."

test2: target
	./$(TARGET) -I/usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed -I /usr/local/include -I /usr/include/x86_64-linux-gnu -I /usr/include -I /usr/include/x86_64-linux-gnu -I /usr/include/c++/4.8 -I /usr/include/c++/4.8/x86_64-linux-gnu -I /usr/include/c++/4.8/backward -I /usr/lib/gcc/x86_64-linux -I src -D "__STDC__ 1" -D "__STDC_VERSION__ 1" src/main.c test.out

doc:
	doxygen Doxyfile
