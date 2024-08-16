/**
 * @file test.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "test.h"

#define LOCAL_MACRO 200
#define COMPLEX_MACRO (HEADER_MACRO + LOCAL_MACRO)

void headerFunction() {
  // Custom implementation for testing
}

void wrong(int) {
  // Simulate incorrect behavior
  // This could be an incorrect calculation or a wrong output
}

void right(int) {
  // Simulate correct behavior
  // This could be a correct calculation or a correct output
}

int main() {
  // Test conditional compilation
  #if defined(LOCAL_MACRO) && (LOCAL_MACRO == 200)
  right(1);
  #else
  wrong(1);
  #endif

  #if (COMPLEX_MACRO == 300)
  right(2);
  #else
  wrong(2);
  #endif

  // Test undefining a macro
  #undef LOCAL_MACRO
  #if defined(LOCAL_MACRO)
  wrong(3);
  #else
  right(3);
  #endif

  // Test including a header file
  headerFunction();

  // Test conditional compilation with undefined macro
  #ifdef UNDEFINED_MACRO
  wrong(4);
  #else
  right(4);
  #endif

  return 0;
}
