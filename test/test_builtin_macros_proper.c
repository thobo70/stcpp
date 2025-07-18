//
// Test for built-in macros __LINE__ and __FILE__
//

// Test 1: Direct usage of __LINE__
int line_number_direct = __LINE__;

// Test 2: Direct usage of __FILE__
const char *filename_direct = __FILE__;

// Test 3: __LINE__ in macro definition
#define GET_LINE __LINE__
int line_in_macro = GET_LINE;

// Test 4: __FILE__ in macro definition
#define GET_FILE __FILE__
const char *file_in_macro = GET_FILE;

// Test 5: Multiple __LINE__ usages on different lines
int line_a = __LINE__;
int line_b = __LINE__;
int line_c = __LINE__;
