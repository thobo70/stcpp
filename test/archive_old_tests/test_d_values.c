/**
 * @file test_d_values.c
 * @brief Test for -D command line option with different value types
 * @author Thomas Boos (tboos70@gmail.com)
 * @copyright Copyright (c) 2024
 */

#ifdef VERSION
int version = VERSION;
#endif

#ifdef MESSAGE
const char *message = MESSAGE;
#endif

#ifdef EMPTY
const char *empty_value = EMPTY;
#endif

int always_here = 1;
