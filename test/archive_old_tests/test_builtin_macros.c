//
// test_builtin_macros.c
// Test for built-in macros __LINE__ and __FILE__
//

// First test: __LINE__ macro
#define CHECK_LINE __LINE__
int line_test = CHECK_LINE;

// Second test: __FILE__ macro
#define CHECK_FILE __FILE__
const char *file_test = CHECK_FILE;

// Third test: Multiple __LINE__ on different lines
int line1 = __LINE__;
int line2 = __LINE__;
int line3 = __LINE__;

// Fourth test: __FILE__ and __LINE__ together
#define FILE_LINE_COMBO __FILE__ ":" __LINE__
const char *combo = FILE_LINE_COMBO;

// Fifth test: __LINE__ in function-like macro
#define REPORT_LINE(x) x __LINE__
int reported = REPORT_LINE(100 +);

// Sixth test: Nested usage
#define OUTER_MACRO(x) x __LINE__
#define INNER_MACRO(y) OUTER_MACRO(y +)
int nested = INNER_MACRO(200);
