/**
 * @file test_simple_stringify.c
 * @brief Simple stringification tests
 */

#define STR(x) #x
#define CONCAT_STR(a, b) #a #b

int main() {
    char *s1 = STR(hello);         // Should be: "hello"
    char *s2 = STR(123);           // Should be: "123"
    char *s3 = STR(a + b);         // Should be: "a + b"
    char *s4 = CONCAT_STR(x, y);   // Should be: "x" "y"
    
    return 0;
}
