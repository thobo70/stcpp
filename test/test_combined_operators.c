/**
 * @file test_combined_operators.c
 * @brief Test stringification and token pasting together
 */

#define MAKE_DEBUG_VAR(name) char name##_debug[] = #name
#define STRINGIFY_CONCAT(a, b) #a #b

int main() {
    MAKE_DEBUG_VAR(counter);           // Should be: char counter_debug[] = "counter";
    char *combined = STRINGIFY_CONCAT(hello, world);  // Should be: char *combined = "hello" "world";
    
    return 0;
}
