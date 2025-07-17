/**
 * @file test_token_pasting.c
 * @brief Token pasting (##) operator tests
 */

// Test basic token pasting
#define CONCAT(a, b) a ## b
#define MAKE_VAR(num) var ## num
#define TRIPLE(a, b, c) a ## b ## c

int main() {
    // Basic concatenation
    int CONCAT(hello, world) = 42;          // Should be: int helloworld = 42;
    
    // Number concatenation
    int MAKE_VAR(1) = 100;                  // Should be: int var1 = 100;
    int MAKE_VAR(42) = 200;                 // Should be: int var42 = 200;
    
    // Multiple concatenation
    int TRIPLE(x, y, z) = 999;              // Should be: int xyz = 999;
    
    // Test results
    return helloworld + var1 + var42 + xyz;
}
