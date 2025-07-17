/**
 * @file debug_test.c
 * @brief Debug test for token pasting
 */

#define SIMPLE(a, b) a ## b

int main() {
    int SIMPLE(test, var) = 1;
    return testvar;
}
