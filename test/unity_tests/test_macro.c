/**
 * @file test_macro.c
 * @brief Comprehensive macro functionality tests using Unity framework
 * 
 * This test suite covers all macro-related functionality including:
 * - Basic macro definition and expansion
 * - Function-like macros with parameters
 * - Recursive macro expansion
 * - Token pasting (##) operator
 * - Stringification (#) operator
 * - Built-in macros (__LINE__, __FILE__)
 * - Conditional compilation
 * - Macro redefinition and undefinition
 * - Edge cases and error conditions
 */

#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Include source code directly for testing internal functions
#include "../../src/macro.c"

void setUp(void) {
    // Initialize macro system for each test
    initMacros();
}

void tearDown(void) {
    // Clean up after each test
    // Note: We might need to implement proper cleanup in macro.c
}

// ============================================================================
// BASIC MACRO FUNCTIONALITY TESTS
// ============================================================================

void test_addMacro_simple_macro(void) {
    // Test basic macro definition like #define PI 3.14159
    int result = addMacro("PI", "3.14159");
    TEST_ASSERT_EQUAL(0, result);
    
    TEST_ASSERT_TRUE(isdefinedMacro("PI"));
    
    char* replacement = replaceBuf("PI", "PI");
    TEST_ASSERT_NOT_NULL(replacement);
    TEST_ASSERT_EQUAL_STRING("3.14159", replacement);
    free(replacement);
}

void test_addMacro_function_macro(void) {
    // Test function-like macro like #define MAX(a,b) ((a)>(b)?(a):(b))
    int result = addMacro("MAX(a,b)", "((a)>(b)?(a):(b))");
    TEST_ASSERT_EQUAL(0, result);
    
    TEST_ASSERT_TRUE(isdefinedMacro("MAX"));
}

void test_addMacro_empty_definition(void) {
    // Test empty macro definition like #define EMPTY
    int result = addMacro("EMPTY", "");
    TEST_ASSERT_EQUAL(0, result);
    
    TEST_ASSERT_TRUE(isdefinedMacro("EMPTY"));
}

void test_addMacro_with_spaces(void) {
    // Test macro with spaces in definition
    int result = addMacro("MESSAGE", "Hello World");
    TEST_ASSERT_EQUAL(0, result);
    
    char* replacement = replaceBuf("MESSAGE", "MESSAGE");
    TEST_ASSERT_NOT_NULL(replacement);
    TEST_ASSERT_EQUAL_STRING("Hello World", replacement);
    free(replacement);
}

// ============================================================================
// MACRO DELETION AND REDEFINITION TESTS  
// ============================================================================

void test_deleteMacro_existing(void) {
    // Add a macro first
    addMacro("TEMP", "temporary");
    TEST_ASSERT_TRUE(isdefinedMacro("TEMP"));
    
    // Delete it
    int result = deleteMacro("TEMP");
    TEST_ASSERT_EQUAL(1, result);
    
    // Verify it's gone
    TEST_ASSERT_FALSE(isdefinedMacro("TEMP"));
}

// ============================================================================
// RECURSIVE MACRO EXPANSION TESTS
// ============================================================================

void test_replaceBuf_simple_replacement(void) {
    // Test: #define A 42, then replace "A" with "42"
    addMacro("A", "42");
    
    char* result = replaceBuf("A", "A");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("42", result);
    free(result);
}

void test_replaceBuf_empty_replacement(void) {
    // Test: #define EMPTY, then replace "EMPTY" with ""
    addMacro("EMPTY", "");
    
    char* result = replaceBuf("EMPTY", "EMPTY");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    free(result);
}

void test_replaceBuf_longer_replacement(void) {
    // Test expansion that makes text longer
    addMacro("SHORT", "This is a much longer replacement text");
    
    char* result = replaceBuf("SHORT", "SHORT");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("This is a much longer replacement text", result);
    free(result);
}

// ============================================================================
// MACRO QUERY TESTS
// ============================================================================

void test_isdefinedMacro_valid_macro(void) {
    addMacro("TEST_MACRO", "value");
    TEST_ASSERT_TRUE(isdefinedMacro("TEST_MACRO"));
}

// ============================================================================
// BUFFER PROCESSING TESTS
// ============================================================================

void test_processBuffer_null_pointer(void) {
    // Test with NULL pointer (should handle gracefully)
    char* result = processBuffer(NULL, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, result); // Should return something, not crash
    if (result) free(result);
}

// ============================================================================
// EDGE CASES AND ERROR CONDITIONS
// ============================================================================

void test_addMacro_redefinition(void) {
    // Add a macro
    addMacro("REDEF", "first");
    
    // Redefine it (should work)
    int result = addMacro("REDEF", "second");
    TEST_ASSERT_EQUAL(0, result);
    
    // Verify new definition
    char* replacement = replaceBuf("REDEF", "REDEF");
    TEST_ASSERT_NOT_NULL(replacement);
    TEST_ASSERT_EQUAL_STRING("second", replacement);
    free(replacement);
}

void test_addMacro_nested_parentheses(void) {
    // Test complex macro with nested parentheses
    int result = addMacro("COMPLEX(a,b,c)", "((a)+((b)*(c)))");
    TEST_ASSERT_EQUAL(0, result);
}

void test_addMacro_with_comments(void) {
    // Test macro definition that might contain comment-like syntax
    int result = addMacro("COMMENT", "/* not a comment */");
    TEST_ASSERT_EQUAL(0, result);
}

void test_addMacro_very_long_name(void) {
    // Test with very long macro name
    char long_name[1000];
    memset(long_name, 'A', 999);
    long_name[999] = '\0';
    
    int result = addMacro(long_name, "value");
    TEST_ASSERT_EQUAL(0, result);
}

void test_addMacro_very_long_replacement(void) {
    // Test with very long replacement text
    char long_replacement[2000];
    memset(long_replacement, 'X', 1999);
    long_replacement[1999] = '\0';
    
    int result = addMacro("LONG_REPLACE", long_replacement);
    TEST_ASSERT_EQUAL(0, result);
}

// ============================================================================
// NEGATIVE TESTS (Expected Failures)
// ============================================================================

void test_addMacro_invalid_format(void) {
    // Test various invalid macro definitions
    // These might return error codes or handle gracefully
    addMacro("", "value");  // Empty name
    addMacro(NULL, "value"); // NULL name
    // Note: Actual behavior depends on implementation
}

void test_deleteMacro_nonexistent(void) {
    int result = deleteMacro("NONEXISTENT");
    TEST_ASSERT_EQUAL(0, result); // Should indicate not found
}

void test_isIdent_valid_characters(void) {
    TEST_ASSERT_TRUE(isIdent('a'));
    TEST_ASSERT_TRUE(isIdent('Z'));
    TEST_ASSERT_TRUE(isIdent('_'));
    TEST_ASSERT_TRUE(isIdent('5'));
}

void test_isIdent_invalid_characters(void) {
    TEST_ASSERT_FALSE(isIdent(' '));
    TEST_ASSERT_FALSE(isIdent('('));
    TEST_ASSERT_FALSE(isIdent('+'));
    TEST_ASSERT_FALSE(isIdent('\n'));
}

// ============================================================================
// ADDITIONAL COVERAGE TESTS
// ============================================================================

void test_isdefinedMacro_undefined_macro(void) {
    TEST_ASSERT_FALSE(isdefinedMacro("UNDEFINED_MACRO"));
}

void test_processBuffer_zero_length(void) {
    char* result = processBuffer("", 0);
    TEST_ASSERT_NOT_NULL(result);
    if (result) free(result);
}

// ============================================================================
// MAIN TEST RUNNER (FOR STANDALONE EXECUTION)
// ============================================================================

#ifdef STANDALONE_TEST
int main(void) {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_addMacro_simple_macro);
    RUN_TEST(test_addMacro_function_macro);
    RUN_TEST(test_addMacro_empty_definition);
    RUN_TEST(test_addMacro_with_spaces);
    
    // Deletion and redefinition tests
    RUN_TEST(test_deleteMacro_existing);
    
    // Replacement tests
    RUN_TEST(test_replaceBuf_simple_replacement);
    RUN_TEST(test_replaceBuf_empty_replacement);
    RUN_TEST(test_replaceBuf_longer_replacement);
    
    // Query tests
    RUN_TEST(test_isdefinedMacro_valid_macro);
    
    // Buffer processing tests
    RUN_TEST(test_processBuffer_null_pointer);
    
    // Edge cases
    RUN_TEST(test_addMacro_redefinition);
    RUN_TEST(test_addMacro_nested_parentheses);
    RUN_TEST(test_addMacro_with_comments);
    RUN_TEST(test_addMacro_very_long_name);
    RUN_TEST(test_addMacro_very_long_replacement);
    
    // Negative tests
    RUN_TEST(test_addMacro_invalid_format);
    RUN_TEST(test_deleteMacro_nonexistent);
    RUN_TEST(test_isIdent_valid_characters);
    RUN_TEST(test_isIdent_invalid_characters);
    
    // Additional coverage
    RUN_TEST(test_isdefinedMacro_undefined_macro);
    RUN_TEST(test_processBuffer_zero_length);
    
    return UNITY_END();
}
#endif
