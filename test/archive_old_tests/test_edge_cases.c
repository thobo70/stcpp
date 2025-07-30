/**
 * @file test_edge_cases.c
 * @brief Edge cases and corner cases for macro processing
 */

// Empty macros
#define EMPTY_MACRO
#define EMPTY_FUNC()

// Macros with no parameters
#define NO_PARAMS() 123
#define ANOTHER_NO_PARAMS() EMPTY_MACRO

// Complex parameter handling
#define STRINGIFY(x) #x
#define CONCAT(a, b) a##b

// Nested parentheses
#define COMPLEX(a, b, c) ((a) + (b) * (c))
#define NESTED_CALL(x) COMPLEX(x, MAX(x, 10), MIN(x, 5))

// Multiple parameters
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#define SUM4(a, b, c, d) ((a) + (b) + (c) + (d))

// Edge case: macro name same as parameter
#define SELF(SELF) (SELF + 1)

// Previously defined helper macros
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main() {
    // Test empty macros
    int test1 = EMPTY_MACRO 42;     // Should be " 42"
    int test2 = NO_PARAMS();        // Should be 123
    int test3 = ANOTHER_NO_PARAMS(); // Should expand EMPTY_MACRO
    
    // Test complex parameters
    int test4 = COMPLEX(1, 2, 3);   // Should be ((1) + (2) * (3))
    int test5 = MAX3(1, 5, 3);      // Should pick 5
    int test6 = SUM4(1, 2, 3, 4);   // Should be ((1) + (2) + (3) + (4))
    
    // Test self-referential
    int test7 = SELF(10);           // Should be (10 + 1)
    
    return 0;
}
