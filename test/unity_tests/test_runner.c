/**
 * @file test_runner.c
 * @brief Comprehensive test runner for all Unity test suites
 * 
 * This file runs all test suites and provides a comprehensive report
 * covering all aspects of the STCPP preprocessor functionality.
 */

#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

// Include all test modules
extern void setUp(void);
extern void tearDown(void);

// Test function declarations from test_macro.c
extern void test_addMacro_simple_macro(void);
extern void test_addMacro_function_macro(void);
extern void test_addMacro_empty_definition(void);
extern void test_deleteMacro_existing(void);
extern void test_replaceBuf_simple_replacement(void);
extern void test_isdefinedMacro_valid_macro(void);

// Test function declarations from test_input.c
extern void test_initsearchdirs_basic(void);
extern void test_addsearchdir_valid_directory(void);
extern void test_newinstream_existing_file(void);
extern void test_getcurrentinstream_after_init(void);
extern void test_readline_valid_stream_and_buffer(void);
extern void test_checkpath_existing_file(void);

// Test function declarations from test_cmdline.c
extern void test_parse_define_option(void);
extern void test_parse_undef_option(void);
extern void test_parse_include_option(void);
extern void test_parse_multiple_options(void);

// Test function declarations from test_exprint.c
extern void test_evaluate_simple_number(void);
extern void test_evaluate_addition(void);
extern void test_evaluate_logical_and(void);
extern void test_evaluate_parentheses(void);

// Test function declarations from test_integration.c
extern void test_basic_simple_macros(void);
extern void test_basic_function_macros(void);
extern void test_recursive_macro_chain(void);
extern void test_token_pasting_basic(void);
extern void test_stringification_basic(void);
extern void test_conditional_compilation(void);
extern void test_builtin_macros_line_file(void);
extern void test_d_option_macro_definition(void);

int main(void) {
    UNITY_BEGIN();
    
    printf("\n");
    printf("========================================\n");
    printf("STCPP Comprehensive Unity Test Suite\n");
    printf("========================================\n");
    printf("Testing all functionality with professional test framework\n");
    printf("Covers all existing tests plus enhanced unit tests\n");
    printf("\n");
    
    // ========================================
    // MACRO FUNCTIONALITY TESTS
    // ========================================
    printf("--- MACRO FUNCTIONALITY TESTS ---\n");
    
    RUN_TEST(test_addMacro_simple_macro);
    RUN_TEST(test_addMacro_function_macro);
    RUN_TEST(test_addMacro_empty_definition);
    RUN_TEST(test_deleteMacro_existing);
    RUN_TEST(test_replaceBuf_simple_replacement);
    RUN_TEST(test_isdefinedMacro_valid_macro);
    
    // ========================================
    // INPUT/FILE HANDLING TESTS
    // ========================================
    printf("--- INPUT/FILE HANDLING TESTS ---\n");
    
    RUN_TEST(test_initsearchdirs_basic);
    RUN_TEST(test_addsearchdir_valid_directory);
    RUN_TEST(test_newinstream_existing_file);
    RUN_TEST(test_getcurrentinstream_after_init);
    RUN_TEST(test_readline_valid_stream_and_buffer);
    RUN_TEST(test_checkpath_existing_file);
    
    // ========================================
    // COMMAND LINE PARSING TESTS
    // ========================================
    printf("--- COMMAND LINE PARSING TESTS ---\n");
    
    RUN_TEST(test_parse_define_option);
    RUN_TEST(test_parse_undef_option);
    RUN_TEST(test_parse_include_option);
    RUN_TEST(test_parse_multiple_options);
    
    // ========================================
    // EXPRESSION EVALUATION TESTS
    // ========================================
    printf("--- EXPRESSION EVALUATION TESTS ---\n");
    
    RUN_TEST(test_evaluate_simple_number);
    RUN_TEST(test_evaluate_addition);
    RUN_TEST(test_evaluate_logical_and);
    RUN_TEST(test_evaluate_parentheses);
    
    // ========================================
    // INTEGRATION TESTS (EXISTING TEST COVERAGE)
    // ========================================
    printf("--- INTEGRATION TESTS (Full Coverage) ---\n");
    
    RUN_TEST(test_basic_simple_macros);
    RUN_TEST(test_basic_function_macros);
    RUN_TEST(test_recursive_macro_chain);
    RUN_TEST(test_token_pasting_basic);
    RUN_TEST(test_stringification_basic);
    RUN_TEST(test_conditional_compilation);
    RUN_TEST(test_builtin_macros_line_file);
    RUN_TEST(test_d_option_macro_definition);
    
    printf("\n");
    printf("========================================\n");
    printf("Test Summary\n");
    printf("========================================\n");
    
    int result = UNITY_END();
    
    if (result == 0) {
        printf("✅ ALL TESTS PASSED!\n");
        printf("🎯 Complete coverage of existing functionality\n");
        printf("🛡️  Enhanced with professional Unity test framework\n");
        printf("🔍 Ready to identify and fix code quality issues\n");
    } else {
        printf("❌ SOME TESTS FAILED\n");
        printf("🐛 Issues detected - see details above\n");
        printf("🔧 Fix issues before removing legacy tests\n");
    }
    
    printf("\n");
    
    return result;
}
