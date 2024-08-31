/**
 * @file exprint.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief evaluates integer expressions, e.g. used for C preprocessor if clauses
 * @version 0.1
 * @date 2024-08-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _EXPRINT_H
#define _EXPRINT_H

typedef long result_t;  // NOLINT

result_t evaluate_expression(const char *expr);

// expression error codes
enum {
  EE_OK = 0,
  EE_INVALDIGIT,
  EE_UNEXPECTEDCHAR,
  EE_MISSINGPAREN,
  EE_MISSINGCOLON,
  EE_DIVBYZERO,
  EE_UNKNOWN
};

extern int expr_error;

#endif