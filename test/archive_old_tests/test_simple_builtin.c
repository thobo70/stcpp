//
// Simple test for built-in macros
//

int current_line = __LINE__;
const char *current_file = __FILE__;

#define TEST_LINE __LINE__
int test_line = TEST_LINE;

#define TEST_FILE __FILE__
const char *test_file = TEST_FILE;
