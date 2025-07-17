/**
 * @file test_include.h
 * @brief Header file for inclusion tests
 */

#ifndef TEST_INCLUDE_H
#define TEST_INCLUDE_H

#define HEADER_VALUE 25
#define DOUBLE_IT(x) ((x) * 2)

// Test recursive macro in header
#define BASE_UNIT 10
#define DERIVED_UNIT BASE_UNIT

#endif // TEST_INCLUDE_H
