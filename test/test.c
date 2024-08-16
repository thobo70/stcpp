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

#define TEST_MACRO(x) ((x) * (x))
#define CONDITIONAL_MACRO(x, y) ((x) > (y) ? (x) : (y))

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

#if defined(HEADER_MACRO)
  #if HEADER_MACRO > 100
    #if HEADER_MACRO < 300
      #if HEADER_MACRO == 150
        // Code for HEADER_MACRO == 150
        wrong(5);
      #else
        // Code for HEADER_MACRO between 100 and 300 but not 150
        wrong(6);
      #endif
    #else
      // Code for HEADER_MACRO >= 300
      wrong(7);
    #endif
  #else
    // Code for HEADER_MACRO <= 100
    right(8);
  #endif
#else
  // Code for HEADER_MACRO not defined
  wrong(8);
#endif

#IF TEST_MACRO(5) != 25
  #error "TEST_MACRO(5) != 25"
#ELSE
  right(9);
#ENDIF

#IF CONDITIONAL_MACRO(5, 10) != 10
  #error "CONDITIONAL_MACRO(5, 10) != 10"
#ELSE
  right(10);
#ENDIF


  return 0;
}
