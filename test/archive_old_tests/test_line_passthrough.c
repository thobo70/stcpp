/* Test for #line directive pass-through to next compiler stage */

#define VERSION 2

#line 100
int function_a() {
    return VERSION * 21;
}

#line 200 "generated.c"
int function_b() {
    return VERSION * 42;
}

#line 50
int function_c() {
    return VERSION + 48;
}

/* Normal code after line directives */
int function_d() {
    return VERSION * 100;
}
