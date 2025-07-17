/**
 * @file test_recursive.c
 * @brief Recursive macro expansion tests
 */

// Simple recursive chain
#define A 42
#define B A
#define C B
#define D C

// Functional recursive macros
#define INNER(x) ((x) + 1)
#define OUTER(x) INNER(x)
#define DOUBLE(x) OUTER(OUTER(x))
#define TRIPLE(x) DOUBLE(OUTER(x))

// Complex recursive definitions
#define VALUE 100
#define OFFSET VALUE
#define TOTAL (OFFSET + 50)

int main() {
    // Test simple recursion
    int test1 = D;  // Should be 42
    
    // Test functional recursion
    int test2 = OUTER(5);    // Should be ((5) + 1)
    int test3 = DOUBLE(10);  // Should be ((((10) + 1)) + 1)
    int test4 = TRIPLE(1);   // Should be ((((((1) + 1)) + 1)) + 1)
    
    // Test complex recursion
    int test5 = TOTAL;  // Should be (100 + 50)
    
    return 0;
}
