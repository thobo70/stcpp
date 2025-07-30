#!/bin/bash
# Test script for -U command line option functionality

cd /home/tom/projects/stcpp

echo "=== Testing -U Command Line Option ==="
echo

# Test 1: Define macros normally
echo "Test 1: Define macros normally"
echo "./bin/stcpp -DDEBUG=1 -DVERSION=42 test/test_u_option.c /tmp/test1.out"
./bin/stcpp -DDEBUG=1 -DVERSION=42 test/test_u_option.c /tmp/test1.out 2>/dev/null
echo "Output:"
cat /tmp/test1.out
echo

# Test 2: Define DEBUG, then undefine it
echo "Test 2: Define DEBUG, then undefine it"
echo "./bin/stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG test/test_u_option.c /tmp/test2.out"
./bin/stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG test/test_u_option.c /tmp/test2.out 2>/dev/null
echo "Output:"
cat /tmp/test2.out
echo

# Test 3: Undefine both macros
echo "Test 3: Undefine both macros"
echo "./bin/stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG -UVERSION test/test_u_option.c /tmp/test3.out"
./bin/stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG -UVERSION test/test_u_option.c /tmp/test3.out 2>/dev/null
echo "Output:"
cat /tmp/test3.out
echo

# Test 4: Try to undefine non-existent macro (should show warning)
echo "Test 4: Try to undefine non-existent macro (should show warning)"
echo "./bin/stcpp -UNONEXISTENT test/test_u_option.c /tmp/test4.out"
./bin/stcpp -UNONEXISTENT test/test_u_option.c /tmp/test4.out 2>&1 | grep -v "CPATH not set"
echo "Output:"
cat /tmp/test4.out
echo

# Test 5: Define, undefine, then redefine
echo "Test 5: Define, undefine, then redefine"
echo "./bin/stcpp -DDEBUG=5 -UDEBUG -DDEBUG=10 test/test_u_option.c /tmp/test5.out"
./bin/stcpp -DDEBUG=5 -UDEBUG -DDEBUG=10 test/test_u_option.c /tmp/test5.out 2>/dev/null
echo "Output:"
cat /tmp/test5.out
echo

# Test 6: Mix -D and -U options
echo "Test 6: Mix -D and -U options with multiple macros"
echo "./bin/stcpp -DDEBUG=1 -DVERSION=2 -DFEATURE=1 -UDEBUG -UFEATURE test/test_u_option.c /tmp/test6.out"
./bin/stcpp -DDEBUG=1 -DVERSION=2 -DFEATURE=1 -UDEBUG -UFEATURE test/test_u_option.c /tmp/test6.out 2>/dev/null
echo "Output:"
cat /tmp/test6.out
echo

echo "=== All -U option tests completed ==="
