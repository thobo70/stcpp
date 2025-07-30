#!/bin/bash
# Test script for -D command line option functionality

cd /home/tom/projects/stcpp

echo "=== Testing -D Command Line Option ==="
echo

# Test 1: Simple macro definition (defaults to 1)
echo "Test 1: Simple macro definition"
echo "./bin/stcpp -DDEBUG test/test_simple_ifdef.c /tmp/test1.out"
./bin/stcpp -DDEBUG test/test_simple_ifdef.c /tmp/test1.out 2>/dev/null
echo "Output:"
cat /tmp/test1.out
echo

# Test 2: Macro with integer value
echo "Test 2: Macro with integer value"
echo "./bin/stcpp -DDEBUG=5 test/test_simple_ifdef.c /tmp/test2.out"
./bin/stcpp -DDEBUG=5 test/test_simple_ifdef.c /tmp/test2.out 2>/dev/null
echo "Output:"
cat /tmp/test2.out
echo

# Test 3: Multiple macros
echo "Test 3: Multiple macros"
echo "./bin/stcpp -DDEBUG -DVERBOSE test/test_simple_ifdef.c /tmp/test3.out"
./bin/stcpp -DDEBUG -DVERBOSE test/test_simple_ifdef.c /tmp/test3.out 2>/dev/null
echo "Output:"
cat /tmp/test3.out
echo

# Test 4: String values
echo "Test 4: String values and empty values"
echo "./bin/stcpp -DVERSION=42 -DMESSAGE=\"hello\" -DEMPTY= test/test_d_values.c /tmp/test4.out"
./bin/stcpp -DVERSION=42 -DMESSAGE=\"hello\" -DEMPTY= test/test_d_values.c /tmp/test4.out 2>/dev/null
echo "Output:"
cat /tmp/test4.out
echo

# Test 5: Complex macro values
echo "Test 5: Complex macro values"
echo "./bin/stcpp -DMAX=100 -DMIN=0 -DRANGE='(MAX-MIN)' test/test_simple_d_option.c /tmp/test5.out"
./bin/stcpp -DMAX=100 -DMIN=0 -DRANGE='(MAX-MIN)' test/test_simple_d_option.c /tmp/test5.out 2>/dev/null
echo "Output:"  
cat /tmp/test5.out
echo

echo "=== All tests completed ==="
