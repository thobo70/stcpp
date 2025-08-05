/**
 * @file version.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Version information implementation for stcpp
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This file implements version information functions that provide
 * Git-based version strings and build information.
 */

#include <stdio.h>
#include "version.h"

const char* get_version_string(void) {
    return VERSION_STRING;
}

const char* get_build_date(void) {
    return BUILD_DATE;
}

void print_version_info(void) {
    printf("stcpp version %s\n", get_version_string());
    printf("Built on %s\n", get_build_date());
}
