//
// Test for -D command line option functionality
//

// Test 1: Simple macro definition without value (should default to 1)
#ifdef SIMPLE
int simple_defined = SIMPLE;
#endif

// Test 2: Macro with integer value
int version = VERSION;

// Test 3: Macro with string value
const char *message = MESSAGE;

// Test 4: Macro with empty value
const char *empty_macro = EMPTY;

// Test 5: Complex macro with multiple tokens
int calculation = COMPLEX;

// Test 6: Conditional compilation based on defined macros
#if defined(FEATURE_A) && FEATURE_A > 0
int feature_a_enabled = 1;
#else
int feature_a_enabled = 0;
#endif

// Test 7: Multiple macro usage
int sum = VALUE1 + VALUE2;
