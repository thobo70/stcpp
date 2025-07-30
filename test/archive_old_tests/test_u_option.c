/**
 * @file test_u_option.c
 * @brief Test for -U command line option functionality
 * @author Thomas Boos (tboos70@gmail.com)
 * @copyright Copyright (c) 2024
 */

// Test code that uses macros that might be defined or undefined
#ifdef DEBUG
int debug_enabled = DEBUG;
#else
int debug_enabled = 0;
#endif

#ifdef VERSION
int version = VERSION;
#else
int version = 0;
#endif

#ifndef FEATURE
int feature_disabled = 1;
#else
int feature_disabled = 0;
#endif

// Always present code
int always_here = 42;
