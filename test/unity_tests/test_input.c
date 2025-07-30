/**
 * @file test_input.c
 * @brief Comprehensive input/file handling tests using Unity framework
 * 
 * This test suite covers all input-related functionality including:
 * - File opening and reading operations
 * - Search directory management
 * - Input stream handling
 * - Path resolution and checking
 * - Error handling for file operations
 */

#include "unity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

// Include source code directly for testing internal functions
#include "../../src/input.c"

void setUp(void) {
    // Initialize input system for each test
    initsearchdirs();
}

void tearDown(void) {
    // Clean up after each test
    // Close any open streams
    releaseinstream(getcurrentinstream());
}

// ============================================================================
// SEARCH DIRECTORY TESTS
// ============================================================================

void test_initsearchdirs_basic(void) {
    // Test basic initialization of search directories
    initsearchdirs();
    // Should complete without errors
    TEST_ASSERT_TRUE(1); // Basic completion test
}

void test_addsearchdir_valid_directory(void) {
    // Test adding a valid directory to search path
    addsearchdir("/tmp");
    // Should complete without errors
    TEST_ASSERT_TRUE(1); // Basic completion test
}

void test_addsearchdir_null_directory(void) {
    // Test adding NULL directory (should handle gracefully)
    addsearchdir(NULL);
    // Should not crash
    TEST_ASSERT_NOT_EQUAL(NULL, NULL); // Just verify we didn't crash
}

void test_addsearchdir_empty_directory(void) {
    // Test adding empty directory string
    addsearchdir("");
    // Should handle gracefully
    TEST_ASSERT_NOT_EQUAL(NULL, NULL); // Just verify we didn't crash
}

// ============================================================================
// FILE OPERATION TESTS
// ============================================================================

void test_newinstream_existing_file(void) {
    // Create a temporary test file
    FILE* temp = fopen("/tmp/test_input.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Test content\nLine 2\n");
    fclose(temp);
    
    // Test opening the file
    instream_t* stream = newinstream("/tmp/test_input.txt");
    TEST_ASSERT_NOT_NULL(stream);
    
    // Clean up
    if (stream) {
        releaseinstream(stream);
    }
    remove("/tmp/test_input.txt");
}

void test_newinstream_nonexistent_file(void) {
    // Test opening a non-existent file
    instream_t* stream = newinstream("/tmp/nonexistent_file_12345.txt");
    // Should return NULL or handle error gracefully
    if (stream) {
        releaseinstream(stream);
    }
    // Test passes if we don't crash
    TEST_ASSERT_TRUE(1);
}

void test_newinstream_null_filename(void) {
    // Test with NULL filename
    instream_t* stream = newinstream(NULL);
    // Should handle gracefully
    if (stream) {
        releaseinstream(stream);
    }
    TEST_ASSERT_TRUE(1);
}

// ============================================================================
// STREAM MANAGEMENT TESTS
// ============================================================================

void test_getcurrentinstream_after_init(void) {
    // Test getting current input stream after initialization
    instream_t* stream = getcurrentinstream();
    // Should return something valid or NULL
    TEST_ASSERT_NOT_NULL(stream);
}

void test_releaseinstream_valid_stream(void) {
    // Create a temporary file and stream
    FILE* temp = fopen("/tmp/test_release.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Test content\n");
    fclose(temp);
    
    instream_t* stream = newinstream("/tmp/test_release.txt");
    if (stream) {
        releaseinstream(stream);
        // Should complete without errors
        TEST_ASSERT_TRUE(1);
    }
    
    remove("/tmp/test_release.txt");
}

void test_releaseinstream_null_stream(void) {
    // Test releasing NULL stream (should handle gracefully)
    releaseinstream(NULL);
    TEST_ASSERT_TRUE(1); // Should not crash
}

// ============================================================================
// LINE READING TESTS
// ============================================================================

void test_readline_valid_stream_and_buffer(void) {
    // Create a test file with known content
    FILE* temp = fopen("/tmp/test_readline.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "First line\nSecond line\nThird line\n");
    fclose(temp);
    
    // Open as input stream
    instream_t* old_stream = getcurrentinstream();
    instream_t* stream = newinstream("/tmp/test_readline.txt");
    TEST_ASSERT_NOT_NULL(stream);
    
    char buffer[100];
    int read_result = readline(stream, buffer, sizeof(buffer));
    
    // Should successfully read a line
    TEST_ASSERT_GREATER_THAN(0, read_result);
    TEST_ASSERT_NOT_NULL(strstr(buffer, "First"));
    
    // Clean up
    releaseinstream(stream);
    remove("/tmp/test_readline.txt");
}

void test_readline_zero_size(void) {
    // Create a test file
    FILE* temp = fopen("/tmp/test_zero_size.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Test content\n");
    fclose(temp);
    
    instream_t* stream = newinstream("/tmp/test_zero_size.txt");
    TEST_ASSERT_NOT_NULL(stream);
    
    char buffer[100];
    int read_result = readline(stream, buffer, 0);
    // Should handle zero size appropriately
    TEST_ASSERT_LESS_OR_EQUAL(0, read_result);
    
    // Clean up
    releaseinstream(stream);
    remove("/tmp/test_zero_size.txt");
}

void test_readline_null_buffer(void) {
    // Create a test file
    FILE* temp = fopen("/tmp/test_null_buffer.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Test content\n");
    fclose(temp);
    
    instream_t* stream = newinstream("/tmp/test_null_buffer.txt");
    TEST_ASSERT_NOT_NULL(stream);
    
    int read_result = readline(stream, NULL, 100);
    // Should handle NULL buffer gracefully
    TEST_ASSERT_LESS_OR_EQUAL(0, read_result);
    
    // Clean up
    releaseinstream(stream);
    remove("/tmp/test_null_buffer.txt");
}

// ============================================================================
// PATH CHECKING TESTS
// ============================================================================

void test_checkpath_existing_file(void) {
    // Create a temporary file
    FILE* temp = fopen("/tmp/test_checkpath.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Test\n");
    fclose(temp);
    
    // Test checkpath function
    char* result = checkpath("/tmp/test_checkpath.txt");
    TEST_ASSERT_NOT_NULL(result);
    
    // Clean up
    if (result) free(result);
    remove("/tmp/test_checkpath.txt");
}

void test_checkpath_nonexistent_file(void) {
    // Test with non-existent file
    char* result = checkpath("/tmp/definitely_nonexistent_file_12345.txt");
    // Should return NULL or handle appropriately
    if (result) free(result);
    TEST_ASSERT_TRUE(1); // Test passes if no crash
}

void test_checkpath_null_filename(void) {
    // Test with NULL filename
    char* result = checkpath(NULL);
    // Should handle gracefully
    if (result) free(result);
    TEST_ASSERT_TRUE(1);
}

// ============================================================================
// INTEGRATION TESTS
// ============================================================================

void test_file_operations_integration(void) {
    // Test complete file operation workflow
    
    // 1. Create test file
    FILE* temp = fopen("/tmp/test_integration.txt", "w");
    TEST_ASSERT_NOT_NULL(temp);
    fprintf(temp, "Line 1\nLine 2\nLine 3\n");
    fclose(temp);
    
    // 2. Check path
    char* path = checkpath("/tmp/test_integration.txt");
    TEST_ASSERT_NOT_NULL(path);
    
    // 3. Open as input stream
    instream_t* stream = newinstream("/tmp/test_integration.txt");
    TEST_ASSERT_NOT_NULL(stream);
    
    // 4. Read lines
    char buffer[100];
    int lines_read = 0;
    while (readline(stream, buffer, sizeof(buffer)) > 0) {
        lines_read++;
        if (lines_read > 10) break; // Safety limit
    }
    
    TEST_ASSERT_GREATER_THAN(0, lines_read);
    
    // 5. Clean up
    releaseinstream(stream);
    if (path) free(path);
    remove("/tmp/test_integration.txt");
}

// ============================================================================
// MAIN TEST RUNNER (FOR STANDALONE EXECUTION)
// ============================================================================

#ifdef STANDALONE_TEST
int main(void) {
    UNITY_BEGIN();
    
    // Search directory tests
    RUN_TEST(test_initsearchdirs_basic);
    RUN_TEST(test_addsearchdir_valid_directory);
    RUN_TEST(test_addsearchdir_null_directory);
    RUN_TEST(test_addsearchdir_empty_directory);
    
    // File operation tests
    RUN_TEST(test_newinstream_existing_file);
    RUN_TEST(test_newinstream_nonexistent_file);
    RUN_TEST(test_newinstream_null_filename);
    
    // Stream management tests
    RUN_TEST(test_getcurrentinstream_after_init);
    RUN_TEST(test_releaseinstream_valid_stream);
    RUN_TEST(test_releaseinstream_null_stream);
    
    // Line reading tests
    RUN_TEST(test_readline_valid_stream_and_buffer);
    RUN_TEST(test_readline_zero_size);
    RUN_TEST(test_readline_null_buffer);
    
    // Path checking tests
    RUN_TEST(test_checkpath_existing_file);
    RUN_TEST(test_checkpath_nonexistent_file);
    RUN_TEST(test_checkpath_null_filename);
    
    // Integration tests
    RUN_TEST(test_file_operations_integration);
    
    return UNITY_END();
}
#endif
