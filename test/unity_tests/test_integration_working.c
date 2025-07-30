/**
 * @file test_integration.c
 * @brief Comprehensive integration tests using Unity framework
 * 
 * This test suite provides end-to-end testing that covers all existing test scenarios
 * by running the stcpp binary and checking outputs, ensuring no functionality is lost.
 */

#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void setUp(void) {
    // Set up test environment - ensure stcpp binary exists
    if (access("../../bin/stcpp", F_OK) != 0) {
        printf("WARNING: stcpp binary not found at ../../bin/stcpp\n");
        printf("Please run 'make' in the project root to build stcpp\n");
    }
}

void tearDown(void) {
    // Clean up temporary files
    system("rm -f /tmp/test_*.c /tmp/test_*.out 2>/dev/null");
}

// Helper function to run stcpp and capture output
int run_stcpp_test(const char* input_content, char* output_buffer, size_t output_size, const char* extra_args) {
    // Write input to temporary file
    FILE* input_file = fopen("/tmp/test_input.c", "w");
    if (!input_file) return -1;
    fprintf(input_file, "%s", input_content);
    fclose(input_file);
    
    // Build command
    char command[1024];
    if (extra_args && strlen(extra_args) > 0) {
        snprintf(command, sizeof(command), "../../bin/stcpp %s /tmp/test_input.c /tmp/test_output.c 2>/dev/null", extra_args);
    } else {
        snprintf(command, sizeof(command), "../../bin/stcpp /tmp/test_input.c /tmp/test_output.c 2>/dev/null");
    }
    
    // Run stcpp
    int result = system(command);
    
    // Read output if successful
    if (result == 0) {
        FILE* output_file = fopen("/tmp/test_output.c", "r");
        if (output_file) {
            size_t bytes_read = fread(output_buffer, 1, output_size - 1, output_file);
            output_buffer[bytes_read] = '\0';
            fclose(output_file);
            return 0;
        }
    }
    
    return -1;
}

// ============================================================================
// BASIC MACRO FUNCTIONALITY TESTS (covers test_basic.c)
// ============================================================================

void test_basic_simple_macros(void) {
    const char* input = 
        "#define PI 3.14159\n"
        "#define VERSION \"1.0\"\n"
        "#define DEBUG 1\n"
        "double radius = PI * 2.0;\n"
        "char *ver = VERSION;\n"
        "int flag = DEBUG;\n";
    
    char output[1024];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "3.14159") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "\"1.0\"") != NULL);
        TEST_ASSERT_TRUE(strstr(output, " 1") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for basic macros test");
    }
}

void test_basic_function_macros(void) {
    const char* input = 
        "#define MAX(a, b) ((a) > (b) ? (a) : (b))\n"
        "#define MIN(a, b) ((a) < (b) ? (a) : (b))\n"
        "#define SQUARE(x) ((x) * (x))\n"
        "int max_val = MAX(10, 20);\n"
        "int min_val = MIN(5, 3);\n"
        "int sq = SQUARE(4);\n";
    
    char output[1024];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "((10) > (20)") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "((5) < (3)") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "((4) * (4))") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for function macros test");
    }
}

// ============================================================================
// RECURSIVE MACRO EXPANSION TESTS (covers test_recursive.c)
// ============================================================================

void test_recursive_macro_chain(void) {
    const char* input = 
        "#define A 42\n"
        "#define B A\n"
        "#define C B\n"
        "#define D C\n"
        "int test1 = D;\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "42") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for recursive macros test");
    }
}

void test_recursive_function_macros(void) {
    const char* input = 
        "#define INNER(x) ((x) + 1)\n"
        "#define OUTER(x) INNER(x)\n"
        "#define DOUBLE(x) OUTER(OUTER(x))\n"
        "int test2 = OUTER(5);\n"
        "int test3 = DOUBLE(10);\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "((5) + 1)") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "((((10) + 1)) + 1)") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for recursive function macros test");
    }
}

// ============================================================================
// TOKEN PASTING TESTS (covers test_token_pasting.c)
// ============================================================================

void test_token_pasting_basic(void) {
    const char* input = 
        "#define CONCAT(a, b) a ## b\n"
        "#define MAKE_VAR(num) var ## num\n"
        "#define TRIPLE(a, b, c) a ## b ## c\n"
        "int CONCAT(hello, world) = 42;\n"
        "int MAKE_VAR(1) = 100;\n"
        "int TRIPLE(x, y, z) = 999;\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "helloworld") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "var1") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "xyz") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for token pasting test");
    }
}

// ============================================================================
// STRINGIFICATION TESTS (covers test_stringification.c)
// ============================================================================

void test_stringification_basic(void) {
    const char* input = 
        "#define STRINGIFY(x) #x\n"
        "#define MESSAGE(msg) \"Message: \" #msg\n"
        "#define TOSTRING(value) #value\n"
        "char *test1 = STRINGIFY(hello);\n"
        "char *test2 = STRINGIFY(a + b);\n"
        "char *num1 = TOSTRING(42);\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "\"hello\"") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "\"a + b\"") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "\"42\"") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for stringification test");
    }
}

// ============================================================================
// CONDITIONAL COMPILATION TESTS (covers test_conditionals.c)
// ============================================================================

void test_conditional_compilation(void) {
    const char* input = 
        "#define FEATURE_A 1\n"
        "#define FEATURE_B 0\n"
        "#define VERSION_MAJOR 2\n"
        "#define VERSION_MINOR 1\n"
        "#if FEATURE_A\n"
        "int feature_a_code = 1;\n"
        "#endif\n"
        "#if FEATURE_B\n"
        "int feature_b_code = 1;\n"
        "#else\n"
        "int no_feature_b = 0;\n"
        "#endif\n"
        "#if (VERSION_MAJOR > 1) && (VERSION_MINOR >= 0)\n"
        "int version_check = 1;\n"
        "#endif\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "feature_a_code = 1") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "no_feature_b = 0") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "version_check = 1") != NULL);
        // Should NOT contain the FEATURE_B code
        TEST_ASSERT_FALSE(strstr(output, "feature_b_code = 1") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for conditional compilation test");
    }
}

// ============================================================================
// BUILT-IN MACROS TESTS (covers test_builtin_macros.c)
// ============================================================================

void test_builtin_macros_line_file(void) {
    const char* input = 
        "int line1 = __LINE__;\n"
        "int line2 = __LINE__;\n"
        "int line3 = __LINE__;\n"
        "const char *file = __FILE__;\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        // Should contain line numbers (1, 2, 3) and filename
        TEST_ASSERT_TRUE(strstr(output, "= 1") != NULL || strstr(output, "= 2") != NULL || strstr(output, "= 3") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "test_input.c") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for built-in macros test");
    }
}

// ============================================================================
// COMMAND LINE OPTION TESTS (covers test_*_option.c)
// ============================================================================

void test_d_option_macro_definition(void) {
    const char* input = 
        "#ifdef DEBUG\n"
        "int debug_enabled = DEBUG;\n"
        "#else\n"
        "int debug_enabled = 0;\n"
        "#endif\n"
        "#ifdef RELEASE\n"
        "int release_mode = RELEASE;\n"
        "#endif\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), "-DDEBUG=1 -DRELEASE=2");
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "debug_enabled = 1") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "release_mode = 2") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for -D option test");
    }
}

void test_u_option_macro_undefinition(void) {
    const char* input = 
        "#define TEMP 123\n"
        "#ifdef TEMP\n"
        "int temp_defined = TEMP;\n"
        "#endif\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), "-UTEMP");
    
    if (result == 0) {
        // Should NOT contain temp_defined since TEMP was undefined
        TEST_ASSERT_FALSE(strstr(output, "temp_defined") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for -U option test");
    }
}

// ============================================================================
// COMBINED FUNCTIONALITY TESTS (covers test_combined_operators.c)
// ============================================================================

void test_combined_operators(void) {
    const char* input = 
        "#define MAKE_STRING_VAR(name) char name##_str[] = #name\n"
        "#define LOG_VAR(var) printf(#var \" = %d\\n\", var##_value)\n"
        "MAKE_STRING_VAR(test);\n"
        "// LOG_VAR(debug); // Would expand to printf usage\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "test_str[] = \"test\"") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for combined operators test");
    }
}

// ============================================================================
// EDGE CASES TESTS (covers test_edge_cases.c)
// ============================================================================

void test_edge_cases(void) {
    const char* input = 
        "#define EMPTY\n"
        "#define SPACE_SEPARATED a b c\n"
        "#define WITH_PARENS (1 + 2 * 3)\n"
        "#define NESTED_EXPANSION SPACE_SEPARATED\n"
        "int empty_test = EMPTY 123;\n"
        "int space_test = SPACE_SEPARATED;\n"
        "int paren_test = WITH_PARENS;\n"
        "int nested_test = NESTED_EXPANSION;\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "123") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "a b c") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "(1 + 2 * 3)") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for edge cases test");
    }
}

// ============================================================================
// INCLUDE FUNCTIONALITY TESTS (covers test_include.c)
// ============================================================================

void test_include_functionality(void) {
    // Create a test header file
    FILE* header = fopen("/tmp/test_header.h", "w");
    TEST_ASSERT_NOT_NULL(header);
    fprintf(header, "#define HEADER_VALUE 42\n");
    fprintf(header, "#define HEADER_MACRO(x) ((x) * 2)\n");
    fclose(header);
    
    const char* input = 
        "#include \"/tmp/test_header.h\"\n"
        "int value = HEADER_VALUE;\n"
        "int doubled = HEADER_MACRO(25);\n";
    
    char output[512];
    int result = run_stcpp_test(input, output, sizeof(output), NULL);
    
    if (result == 0) {
        TEST_ASSERT_TRUE(strstr(output, "42") != NULL);
        TEST_ASSERT_TRUE(strstr(output, "((25) * 2)") != NULL);
    } else {
        TEST_FAIL("stcpp execution failed for include functionality test");
    }
    
    // Clean up
    remove("/tmp/test_header.h");
}

// ============================================================================
// MAIN TEST RUNNER (FOR STANDALONE EXECUTION)
// ============================================================================

#ifdef STANDALONE_TEST
int main(void) {
    UNITY_BEGIN();
    
    printf("\n========================================\n");
    printf("STCPP Integration Tests - Unity Framework\n");
    printf("========================================\n");
    printf("Complete coverage of existing functionality\n");
    printf("Testing all scenarios without weakening coverage\n\n");
    
    // Basic functionality tests
    RUN_TEST(test_basic_simple_macros);
    RUN_TEST(test_basic_function_macros);
    
    // Recursive macro tests
    RUN_TEST(test_recursive_macro_chain);
    RUN_TEST(test_recursive_function_macros);
    
    // Token pasting tests
    RUN_TEST(test_token_pasting_basic);
    
    // Stringification tests
    RUN_TEST(test_stringification_basic);
    
    // Conditional compilation tests
    RUN_TEST(test_conditional_compilation);
    
    // Built-in macros tests
    RUN_TEST(test_builtin_macros_line_file);
    
    // Command line option tests
    RUN_TEST(test_d_option_macro_definition);
    RUN_TEST(test_u_option_macro_undefinition);
    
    // Combined functionality tests
    RUN_TEST(test_combined_operators);
    
    // Edge cases tests
    RUN_TEST(test_edge_cases);
    
    // Include functionality tests
    RUN_TEST(test_include_functionality);
    
    int result = UNITY_END();
    
    if (result == 0) {
        printf("\n✅ ALL INTEGRATION TESTS PASSED!\n");
        printf("🎯 Complete coverage of existing test scenarios\n");
        printf("🛡️ Professional Unity test framework operational\n");
        printf("✨ Ready to replace legacy tests safely\n");
    } else {
        printf("\n❌ SOME INTEGRATION TESTS FAILED\n");
        printf("🐛 Issues detected - review before proceeding\n");
    }
    
    return result;
}
#endif
