// Test file for #line directive functionality

#define MESSAGE(x) Line: __LINE__, File: __FILE__, Message: x

// Initial line numbers should be normal
MESSAGE("before line directive")

#line 100
MESSAGE("after line 100")

#line 200 "custom.c"
MESSAGE("after line 200 in custom.c")

#line 50
MESSAGE("after line 50")

// Test with different formatting
#line    300    "spaced.c"   
MESSAGE("after line 300 in spaced.c")

#line 1 "final.c"
MESSAGE("back to line 1 in final.c")
