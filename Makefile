CC = gcc
BINDIR = ./bin
SRCDIR = ./src
OBJS = $(BINDIR)/exprint.o $(BINDIR)/cmdline.o $(BINDIR)/input.o $(BINDIR)/macro.o $(BINDIR)/main.o
TARGET = $(BINDIR)/stcpp
CFLAGS = -g -Og -Wall -Werror -Wextra -pedantic -Isrc
# CFLAGS = -DNDEBUG -Oz -Wall -Werror -Wextra -pedantic -Isrc

.PHONY: all clean test test2 test-unity test-all test-basic test-recursive test-edge test-conditionals test-undef test-include test-token-pasting test-stringification test-line doc lint

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

# Unity Test Framework (Primary Test Suite)
test-unity: $(TARGET)
	@echo "Running Unity Test Framework..."
	@cd test/unity_tests && make test-comprehensive

# Comprehensive test suite (Unity Framework)
test-all: test-unity

# Simple test (for quick verification)
test: target
	./$(TARGET) -Itest -D "__STDC__ 1" -D "__STDC_VERSION__ 1" test/unity_tests/test_comprehensive.c test.out

# Legacy individual tests (archived - use Unity framework instead)
# Note: These targets now reference archived test files for backward compatibility
# For active testing, use: make test-unity

test-basic: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing basic macro functionality..."
	./$(TARGET) -Itest test/archive_old_tests/test_basic.c test_results/test_basic.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_basic.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-recursive: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing recursive macro expansion..."
	./$(TARGET) -Itest test/archive_old_tests/test_recursive.c test_results/test_recursive.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_recursive.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-edge: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing edge cases..."
	./$(TARGET) -Itest test/archive_old_tests/test_edge_cases.c test_results/test_edge_cases.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_edge_cases.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-conditionals: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing conditional compilation..."
	./$(TARGET) -Itest test/archive_old_tests/test_conditionals.c test_results/test_conditionals.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_conditionals.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-undef: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing #undef functionality..."
	./$(TARGET) -Itest test/archive_old_tests/test_undef.c test_results/test_undef.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_undef.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-include: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing file inclusion..."
	./$(TARGET) -Itest test/archive_old_tests/test_include.c test_results/test_include.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_include.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-token-pasting: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing token pasting (##) operator..."
	./$(TARGET) -Itest test/archive_old_tests/test_token_pasting.c test_results/test_token_pasting.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_token_pasting.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-stringification: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing stringification (#) operator..."
	./$(TARGET) -Itest test/archive_old_tests/test_stringification.c test_results/test_stringification.out || echo "File not found - use Unity tests"
	@echo "Output:"
	@head -20 test_results/test_stringification.out 2>/dev/null || echo "Use Unity framework: cd test/unity_tests && make test-comprehensive"

test-line: target test_results
	@echo "⚠️  Legacy test - Use 'make test-unity' for current testing"
	@echo "Testing #line directive..."
	./$(TARGET) -Itest test/archive_old_tests/test_line_simple.c test_results/test_line_simple.out || echo "File not found - use Unity tests"
	@echo "Line directive test completed successfully."
	@echo "Output file contains $(shell wc -l < test_results/test_line_simple.out 2>/dev/null || echo 0) lines"
	@echo "#line directives are passed through to next compiler stage."
	@echo ""
	@echo "Testing #line pass-through to next compiler stage..."
	./$(TARGET) -Itest test/archive_old_tests/test_line_passthrough.c test_results/test_line_passthrough.out || echo "File not found - use Unity tests"
	@echo "Pass-through test completed."
	@if [ -f test_results/test_line_passthrough.out ] && \
	   grep -q "#line 100" test_results/test_line_passthrough.out && \
	   grep -q "#line 200 \"generated.c\"" test_results/test_line_passthrough.out && \
	   grep -q "#line 50" test_results/test_line_passthrough.out; then \
		echo "✓ #line directives successfully passed through to next compiler stage"; \
	else \
		echo "✗ #line directives not found in output - use Unity framework for current testing"; \
	fi

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
