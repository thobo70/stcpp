/**
 * @file test_simple_pasting.c
 * @brief Simple token pasting tests
 */

// Simple test cases
#define CONCAT(a, b) a ## b
#define PREFIX(name) my_ ## name

int main() {
    int CONCAT(hello, world) = 42;     // Should become: int helloworld = 42;
    int PREFIX(variable) = 10;         // Should become: int my_variable = 10;
    
    return helloworld + my_variable;
}
