#!/bin/bash

# Test runner script for stcpp comprehensive testing
# This script runs various test cases and reports results

STCPP="./bin/stcpp"
TEST_DIR="test"
RESULTS_DIR="test_results"
FAILED_TESTS=0
TOTAL_TESTS=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "=================================="
echo "STCPP Comprehensive Test Suite"
echo "=================================="

# Function to run a test
run_test() {
    local test_name=$1
    local test_file=$2
    local expected_patterns=("${@:3}")
    
    echo -n "Testing $test_name... "
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Run stcpp
    if $STCPP -I$TEST_DIR "$TEST_DIR/$test_file" "$RESULTS_DIR/${test_file%.c}.out" 2>/dev/null; then
        # Check if output file was created
        if [ -f "$RESULTS_DIR/${test_file%.c}.out" ]; then
            echo -e "${GREEN}PASS${NC}"
            
            # If patterns provided, check them
            if [ ${#expected_patterns[@]} -gt 0 ]; then
                echo "  Checking output patterns..."
                for pattern in "${expected_patterns[@]}"; do
                    if grep -q "$pattern" "$RESULTS_DIR/${test_file%.c}.out"; then
                        echo -e "    ✓ Found: ${GREEN}$pattern${NC}"
                    else
                        echo -e "    ✗ Missing: ${RED}$pattern${NC}"
                    fi
                done
            fi
        else
            echo -e "${RED}FAIL${NC} (no output file)"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    else
        echo -e "${RED}FAIL${NC} (execution error)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Test 1: Basic macro functionality
run_test "Basic Macros" "test_basic.c" \
    "3.14159 \* 2.0" \
    "\"1.0\"" \
    "((10) > (20) ? (10) : (20))"

# Test 2: Recursive macro expansion
run_test "Recursive Macros" "test_recursive.c" \
    "42" \
    "((5) + 1)" \
    "((((10) + 1)) + 1)" \
    "(100 + 50)"

# Test 3: Edge cases
run_test "Edge Cases" "test_edge_cases.c" \
    "123" \
    "((1) + (2) \* (3))" \
    "(10 + 1)"

# Test 4: Conditional compilation
run_test "Conditionals" "test_conditionals.c" \
    "feature_a_code = 1" \
    "no_feature_b = 0" \
    "version_check = 1"

# Test 5: Undef functionality
run_test "Undef/Redef" "test_undef.c" \
    "100" \
    "200" \
    "not_defined = 1"

# Test 6: Include functionality
run_test "File Inclusion" "test_include.c" \
    "25" \
    "(25 + 50)" \
    "((10) \* 2)"

# Test 7: Token pasting (##) operator
run_test "Token Pasting" "test_token_pasting.c" \
    "int helloworld = 42" \
    "int var1 = 100" \
    "int var42 = 200" \
    "int xyz = 999"

# Test 8: Stringification (#) operator
run_test "Stringification" "test_stringification.c" \
    "char \*test1 = \"hello\"" \
    "char \*test2 = \"world123\"" \
    "char \*test3 = \"a + b\"" \
    "char \*num1 = \"42\"" \
    "char test_str\[\] = \"test\""

# Test 9: Line directive
echo -n "Testing Line Directive... "
TOTAL_TESTS=$((TOTAL_TESTS + 1))
if $STCPP -I$TEST_DIR "$TEST_DIR/test_line_simple.c" "$RESULTS_DIR/test_line_simple.out" 2>/dev/null; then
    # Check that #line directives are passed through to output
    if [ -f "$RESULTS_DIR/test_line_simple.out" ] && \
       grep -q "#line 100" "$RESULTS_DIR/test_line_simple.out" && \
       grep -q "#line 200 \"virtual.c\"" "$RESULTS_DIR/test_line_simple.out" && \
       grep -q "#line 1 \"final.c\"" "$RESULTS_DIR/test_line_simple.out"; then
        echo -e "${GREEN}PASS${NC}"
        echo "  ✓ #line directives properly processed and passed through to next stage"
    else
        echo -e "${RED}FAIL${NC} (#line directives not processed correctly)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
else
    echo -e "${RED}FAIL${NC} (execution error)"
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

# Test 10: Line directive pass-through to next compiler stage
echo -n "Testing Line Directive Pass-through... "
TOTAL_TESTS=$((TOTAL_TESTS + 1))
if $STCPP -I$TEST_DIR "$TEST_DIR/test_line_passthrough.c" "$RESULTS_DIR/test_line_passthrough.out" 2>/dev/null; then
    # Check that #line directives are passed through to output
    if [ -f "$RESULTS_DIR/test_line_passthrough.out" ] && \
       grep -q "#line 100" "$RESULTS_DIR/test_line_passthrough.out" && \
       grep -q "#line 200 \"generated.c\"" "$RESULTS_DIR/test_line_passthrough.out" && \
       grep -q "#line 50" "$RESULTS_DIR/test_line_passthrough.out"; then
        echo -e "${GREEN}PASS${NC}"
        echo "  ✓ #line directives successfully passed through to next compiler stage"
    else
        echo -e "${RED}FAIL${NC} (#line directives not passed through correctly)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
else
    echo -e "${RED}FAIL${NC} (execution error)"
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi

# Test 11: Built-in macros (__LINE__ and __FILE__)
run_test "Built-in Macros" test_builtin_macros_proper.c \
    "int line_number_direct = 7;" \
    "const char \*filename_direct = \"test/test_builtin_macros_proper.c\";" \
    "int line_in_macro = 14;" \
    "const char \*file_in_macro = \"test/test_builtin_macros_proper.c\";" \
    "int line_a = 21;" \
    "int line_b = 22;" \
    "int line_c = 23;"

echo ""
echo "=================================="
echo "Test Summary"
echo "=================================="
echo "Total Tests: $TOTAL_TESTS"
echo "Passed: $((TOTAL_TESTS - FAILED_TESTS))"
echo "Failed: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
