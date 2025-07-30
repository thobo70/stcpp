/**
 * @file test_stringification.c
 * @brief Stringification (#) operator tests
 */

// Test basic stringification
#define STRINGIFY(x) #x
#define MESSAGE(msg) "Message: " #msg

// Test with different types of content
#define TOSTRING(value) #value
#define DEBUG_PRINT(var) printf("var = " #var " = %d\n", var)

// Test combined with other operators
#define MAKE_STRING_VAR(name) char name##_str[] = #name

int main() {
    // Basic stringification
    char *test1 = STRINGIFY(hello);           // Should be: char *test1 = "hello";
    char *test2 = STRINGIFY(world123);        // Should be: char *test2 = "world123";
    
    // Stringification with spaces and symbols
    char *test3 = STRINGIFY(a + b);           // Should be: char *test3 = "a + b";
    char *test4 = STRINGIFY(x * y / z);       // Should be: char *test4 = "x * y / z";
    
    // Message macro
    char *msg1 = MESSAGE(Error occurred);     // Should be: char *msg1 = "Message: " "Error occurred";
    char *msg2 = MESSAGE(Success);            // Should be: char *msg2 = "Message: " "Success";
    
    // Numbers and expressions
    char *num1 = TOSTRING(42);                // Should be: char *num1 = "42";
    char *expr1 = TOSTRING(1 + 2 * 3);        // Should be: char *expr1 = "1 + 2 * 3";
    
    // Combined with token pasting
    MAKE_STRING_VAR(test);                    // Should be: char test_str[] = "test";
    
    return 0;
}
