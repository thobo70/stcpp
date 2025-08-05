CC = gcc
BINDIR = ./bin
SRCDIR = ./src
OBJS = $(BINDIR)/exprint.o $(BINDIR)/cmdline.o $(BINDIR)/input.o $(BINDIR)/macro.o $(BINDIR)/main.o $(BINDIR)/version.o
TARGET = $(BINDIR)/stcpp

# Extract version information from Git
VERSION := $(shell git describe --tags --always --dirty --match "v*" 2>/dev/null || echo "unknown")
BUILD_DATE := $(shell date -u +"%Y-%m-%d %H:%M:%S UTC" 2>/dev/null || echo "unknown")

CFLAGS = -g -Og -Wall -Werror -Wextra -pedantic -Isrc -DVERSION_STRING='"$(VERSION)"' -DBUILD_DATE='"$(BUILD_DATE)"'
#CFLAGS = -Oz -Wall -Werror -Wextra -pedantic -Isrc

.PHONY: all clean test test2 test-all test-basic test-recursive test-edge test-conditionals test-undef test-include test-token-pasting test-stringification test-line doc lint doc-serve doc-stop help version

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
	@echo "Running Unity comprehensive test suite..."
	@cd test/unity_tests && $(MAKE) test-all

# Unity comprehensive tests
test-unity: target
	@echo "Running Unity comprehensive test suite..."
	@cd test/unity_tests && $(MAKE) test-comprehensive

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

test-line: target test_results
	@echo "Testing #line directive..."
	./$(TARGET) -Itest test/test_line_simple.c test_results/test_line_simple.out
	@echo "Line directive test completed successfully."
	@echo "Output file contains $(shell wc -l < test_results/test_line_simple.out) lines"
	@echo "#line directives are passed through to next compiler stage."
	@echo ""
	@echo "Testing #line pass-through to next compiler stage..."
	./$(TARGET) -Itest test/test_line_passthrough.c test_results/test_line_passthrough.out
	@echo "Pass-through test completed."
	@if grep -q "#line 100" test_results/test_line_passthrough.out && \
	   grep -q "#line 200 \"generated.c\"" test_results/test_line_passthrough.out && \
	   grep -q "#line 50" test_results/test_line_passthrough.out; then \
		echo "✓ #line directives successfully passed through to next compiler stage"; \
	else \
		echo "✗ #line directives not found in output"; \
		exit 1; \
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

# Serve documentation with HTTP server
doc-serve: doc
	@echo "Starting HTTP server for documentation at http://localhost:8080"
	@cd doc/html && python3 -m http.server 8080 > /dev/null 2>&1 &
	@echo "Documentation server started in background"
	@echo "Visit: http://localhost:8080"
	@echo "Stop with: make doc-stop"

# Stop documentation HTTP server
doc-stop:
	@echo "Stopping documentation HTTP server..."
	@pkill -f "python3 -m http.server 8080" || echo "No HTTP server running on port 8080"

# Display help information
help:
	@echo "Super Tiny C Preprocessor (stcpp) - Makefile Help"
	@echo "=================================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all, target    - Build the stcpp binary"
	@echo "  clean          - Remove build artifacts and generated files"
	@echo ""
	@echo "Testing Targets:"
	@echo "  test           - Run Unity comprehensive test suite"
	@echo "  test-unity     - Run Unity comprehensive test suite"
	@echo "  test-all       - Run all tests via test script"
	@echo "  test-basic     - Test basic macro functionality"
	@echo "  test-recursive - Test recursive macro expansion"
	@echo "  test-edge      - Test edge cases"
	@echo "  test-conditionals - Test conditional compilation"
	@echo "  test-undef     - Test #undef functionality"
	@echo "  test-include   - Test file inclusion"
	@echo "  test-token-pasting - Test token pasting (##) operator"
	@echo "  test-stringification - Test stringification (#) operator"
	@echo "  test-line      - Test #line directive"
	@echo ""
	@echo "Documentation Targets:"
	@echo "  doc            - Generate HTML documentation with Doxygen"
	@echo "  doc-serve      - Generate and serve documentation at http://localhost:8080"
	@echo "  doc-stop       - Stop the documentation HTTP server"
	@echo ""
	@echo "Code Quality Targets:"
	@echo "  lint           - Run cpplint on source files"
	@echo ""
	@echo "Utility Targets:"
	@echo "  version        - Display current version information"
	@echo ""
	@echo "Usage Examples:"
	@echo "  make           - Build the preprocessor"
	@echo "  make test      - Run comprehensive test suite"
	@echo "  make doc-serve - Generate and view documentation in browser"
	@echo "  make clean     - Clean up all generated files"
	@echo ""
	@echo "For more information, see README.md"

# Display version information
version:
	@echo "stcpp version $(VERSION)"
	@echo "Built on $(BUILD_DATE)"
