/**
 * @file test_include.c
 * @brief Test file inclusion and macro interaction
 */

#include "test_include.h"

#define LOCAL_VALUE 50
#define COMBINED (HEADER_VALUE + LOCAL_VALUE)

// Test macro from included file
int test_header_macro() {
    return HEADER_VALUE;  // Should be 25 (from header)
}

// Test combination of macros
int test_combined() {
    return COMBINED;  // Should be (25 + 50) = 75
}

// Test function-like macro from header
int test_header_function() {
    return DOUBLE_IT(10);  // Should be ((10) * 2) = 20
}

int main() {
    int header_test = test_header_macro();
    int combined_test = test_combined();
    int function_test = test_header_function();
    
    return 0;
}
