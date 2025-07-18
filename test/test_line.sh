#!/bin/bash

# Test script for #line directive

echo "Testing #line directive..."

# Run the preprocessor on the test file
./bin/stcpp test/test_line.c test_results/test_line.out

# Check if the output was created
if [ ! -f test_results/test_line.out ]; then
    echo "ERROR: Output file not created"
    exit 1
fi

echo "Output generated successfully:"
echo "================================"
cat test_results/test_line.out
echo "================================"

# Verify that line numbers and filenames are being set correctly
# This is a basic test - in a real implementation you'd check specific line numbers
if grep -q "Line.*100" test_results/test_line.out && \
   grep -q "custom.c" test_results/test_line.out && \
   grep -q "Line.*50" test_results/test_line.out && \
   grep -q "spaced.c" test_results/test_line.out && \
   grep -q "final.c" test_results/test_line.out; then
    echo "PASS: #line directive test passed - all expected patterns found"
    exit 0
else
    echo "FAIL: #line directive test failed - expected patterns not found"
    exit 1
fi
