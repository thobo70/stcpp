/**
 * @file test_undef.c
 * @brief Test #undef directive and macro redefinition
 */

#define TEMP_MACRO 100

int function1() {
    int x = TEMP_MACRO;  // Should be 100
    
    #undef TEMP_MACRO
    
    // TEMP_MACRO should no longer be defined
    #ifdef TEMP_MACRO
    int still_defined = 1;  // Should not be included
    #else
    int not_defined = 1;    // Should be included
    #endif
    
    return x;
}

// Redefine the macro
#define TEMP_MACRO 200

int function2() {
    int y = TEMP_MACRO;  // Should be 200
    return y;
}

// Test multiple undef/redef cycles
#define CYCLE_MACRO 1
#undef CYCLE_MACRO
#define CYCLE_MACRO 2
#undef CYCLE_MACRO
#define CYCLE_MACRO 3

int main() {
    int result1 = function1();
    int result2 = function2();
    int cycle = CYCLE_MACRO;  // Should be 3
    
    return 0;
}
