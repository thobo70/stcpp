/**
 * @file exprint.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Header file for expression evaluation functionality.
 * @version 0.1
 * @date 2024-08-31
 * 
 * @copyright Copyright (c) 2024
 * 
 * This header file contains function declarations and type definitions for
 * evaluating integer expressions used in C preprocessor conditional
 * compilation directives (#if, #elif). Supports arithmetic, bitwise,
 * logical, and comparison operators with C-like precedence.
 */
#ifndef _EXPRINT_H
#define _EXPRINT_H

/**
 * @typedef result_t
 * @brief Type for expression evaluation results.
 * 
 * Represents the result of expression evaluation as a signed long integer.
 * This provides sufficient range for most preprocessor constant expressions.
 */
typedef long result_t;  // NOLINT

/**
 * @brief Evaluates an integer expression string.
 * 
 * Parses and evaluates a C-like integer expression with support for:
 * - Arithmetic operators: +, -, *, /, %
 * - Bitwise operators: &, |, ^, ~, <<, >>
 * - Logical operators: &&, ||, !
 * - Comparison operators: <, <=, >, >=, ==, !=
 * - Ternary operator: ? :
 * - Parentheses for grouping
 * - Integer literals (decimal, hexadecimal, octal, binary)
 * - Character constants
 * 
 * @param expr Null-terminated string containing the expression to evaluate.
 * @return The result of the expression evaluation, or 0 if an error occurred.
 * @see expr_error for error status after evaluation.
 */
result_t evaluate_expression(const char *expr);

/**
 * @brief Expression evaluation error codes.
 * 
 * These constants define the possible error conditions that can occur
 * during expression evaluation. Check expr_error after calling
 * evaluate_expression() to determine if an error occurred.
 */
enum {
  EE_OK = 0,            /**< No error occurred. */
  EE_INVALDIGIT,        /**< Invalid digit for number base. */
  EE_UNEXPECTEDCHAR,    /**< Unexpected character in expression. */
  EE_MISSINGPAREN,      /**< Missing closing parenthesis. */
  EE_MISSINGCOLON,      /**< Missing ':' in ternary expression. */
  EE_DIVBYZERO,         /**< Division by zero attempted. */
  EE_UNKNOWN            /**< Unknown/unspecified error. */
};

/**
 * @brief Global variable containing the last expression evaluation error.
 * 
 * This variable is set to one of the EE_* constants after each call to
 * evaluate_expression(). Check this value to determine if an error
 * occurred during evaluation.
 */
extern int expr_error;

#endif