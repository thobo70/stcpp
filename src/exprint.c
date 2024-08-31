/**
 * @file exprint.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-31
 * 
 * @copyright Copyright (c) 2024
 * 
 * Integer constants.
 * Character constants, which are interpreted as they would be in normal code.
 * Arithmetic operators for addition, subtraction, multiplication, division,
 * bitwise operations, shifts, comparisons, and logical operations (&& and ||).
 * 
 * 1. ()
 * 2. ! ~ + - (unary)
 * 3. * / %
 * 4. + -
 * 5. << >>
 * 6. < <= > >=
 * 7. == !=
 * 8. &
 * 9. ^
 * 10. |
 * 11. &&
 * 12. ||
 * 13. ?:
 */

#include <ctype.h>

#include "exprint.h"

#ifndef TESTMAIN
#define FENTRY(param)
#define FEXIT(result, left)
#else
#include <stdio.h>
#define FENTRY(param) printf("Entering %s param \"%s\"\n", __func__, param)
#define FEXIT(result, left) printf("Exiting %s with result %ld \"%s\"\n", __func__, result, left)
#endif

// Function prototypes
result_t evaluate_expression(const char *expr);
result_t parse_ternary(const char **expr);
result_t parse_logical_or(const char **expr);
result_t parse_logical_and(const char **expr);
result_t parse_bitwise_or(const char **expr);
result_t parse_bitwise_xor(const char **expr);
result_t parse_bitwise_and(const char **expr);
result_t parse_equality(const char **expr);
result_t parse_relational(const char **expr);
result_t parse_shift(const char **expr);
result_t parse_additive(const char **expr);
result_t parse_multiplicative(const char **expr);
result_t parse_unary(const char **expr);
result_t parse_primary(const char **expr);
result_t parse_number(const char **expr);
result_t parse_char_constant(const char **expr);

int expr_error = 0;

// Main evaluation function
result_t evaluate_expression(const char *expr) {
  expr_error = 0;
  return parse_ternary(&expr);
}

// Parsing functions
result_t parse_ternary(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_logical_or(expr);
  if (**expr == '?') {
    (*expr)++;
    result_t true_expr = parse_ternary(expr);
    if (**expr == ':') {
      (*expr)++;
      result_t false_expr = parse_ternary(expr);
      result = result ? true_expr : false_expr;
    } else {  // Error handling for missing colon
      expr_error = EE_MISSINGCOLON;
      result = 0;  // Indicate an error
    }
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_logical_or(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_logical_and(expr);
  while (**expr == '|' && *(*expr + 1) == '|') {
    (*expr) += 2;
    result_t rhs = parse_ternary(expr);
    result = result || rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_logical_and(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_bitwise_or(expr);
  while (**expr == '&' && *(*expr + 1) == '&') {
    (*expr) += 2;
    result_t rhs = parse_ternary(expr);
    result = result && rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_bitwise_or(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_bitwise_xor(expr);
  while (**expr == '|' && *(*expr + 1) != '|') {
    (*expr)++;
    result_t rhs = parse_ternary(expr);
    result |= rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_bitwise_xor(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_bitwise_and(expr);
  while (**expr == '^') {
    (*expr)++;
    result_t rhs = parse_ternary(expr);
    result ^= rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_bitwise_and(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_equality(expr);
  while (**expr == '&' && *(*expr + 1) != '&') {
    (*expr)++;
    result_t rhs = parse_ternary(expr);
    result &= rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_equality(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_relational(expr);
  while (**expr == '=' || **expr == '!') {
    char op = **expr;
    (*expr)++;
    if (**expr == '=') {
      (*expr)++;
      result_t rhs = parse_ternary(expr);
      if (op == '=') result = result == rhs;
      else if (op == '!') result = result != rhs;
    }
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_relational(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_shift(expr);
  while (**expr == '<' || **expr == '>') {
    char op = **expr;
    (*expr)++;
    if (**expr == '=') {
      (*expr)++;
      result_t rhs = parse_ternary(expr);
      if (op == '<') result = result <= rhs;
      else if (op == '>') result = result >= rhs;
    } else {
      result_t rhs = parse_ternary(expr);
      if (op == '<') result = result < rhs;
      else if (op == '>') result = result > rhs;
    }
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_shift(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_additive(expr);
  while (**expr == '<' || **expr == '>') {
    char op = **expr;
    (*expr)++;
    if (**expr == op) {
      (*expr)++;
      result_t rhs = parse_ternary(expr);
      if (op == '<') result <<= rhs;
      else if (op == '>') result >>= rhs;
    } else {
      (*expr)--;
      break;
    }
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_additive(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_multiplicative(expr);
  while (**expr == '+' || **expr == '-') {
    char op = **expr;
    (*expr)++;
    result_t rhs = parse_ternary(expr);
    if (op == '+') result += rhs;
    else if (op == '-') result -= rhs;
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_multiplicative(const char **expr) {
  FENTRY(*expr);
  result_t result = parse_unary(expr);
  while (**expr == '*' || **expr == '/' || **expr == '%') {
    char op = **expr;
    (*expr)++;
    result_t rhs = parse_ternary(expr);
    if (op == '*') result *= rhs;
    else if (op == '%') result %= rhs;
    else if (op == '/') {  // NOLINT
      if (rhs == 0) {  // Error handling for division by zero
        expr_error = EE_DIVBYZERO;
        result = 0;  // Indicate an error
        break;
      }
      result /= rhs;
    }
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_unary(const char **expr) {
  FENTRY(*expr);
  result_t result = 0;
  if (**expr == '+' || **expr == '-' || **expr == '!' || **expr == '~') {
    char op = **expr;
    (*expr)++;
    result_t rhs = parse_primary(expr);
    if (op == '+') result = rhs;
    else if (op == '-') result = -rhs;
    else if (op == '!') result = !rhs;
    else if (op == '~') result = ~rhs;
  } else {
    result = parse_primary(expr);
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_primary(const char **expr) {
  FENTRY(*expr);
  result_t result;
  while (isspace(**expr)) (*expr)++;
  if (**expr == '(') {
    (*expr)++;
    result = parse_ternary(expr);
    if (**expr == ')') {
      (*expr)++;
    } else {  // Error handling for missing closing parenthesis
      expr_error = EE_MISSINGPAREN;
      result = 0;  // Indicate an error
    }
  } else if (isdigit(**expr)) {
    result = parse_number(expr);
  } else if (**expr == '\'') {
    result = parse_char_constant(expr);
  } else {  // Error handling for unexpected character
    expr_error = EE_UNEXPECTEDCHAR;
    result = 0;  // Indicate an error
  }
  FEXIT(result, *expr);
  return result;
}

result_t parse_number(const char **expr) {
  FENTRY(*expr);
  result_t result = 0;
  int base = 10;
  if (**expr == '0') {
    (*expr)++;
    if (**expr == 'x' || **expr == 'X') {
      base = 16;
      (*expr)++;
    } else if (**expr == 'b' || **expr == 'B') {
      base = 2;
      (*expr)++;
    } else {
      base = 8;
    }
  }
  while (isxdigit(**expr)) {
    int digit = isdigit(**expr) ? **expr - '0' : tolower(**expr) - 'a' + 10;
    if (digit >= base) {  // Error handling for invalid digit
      result = 0;
      expr_error = EE_INVALDIGIT;
      break;
    }
    result = result * base + digit;
    (*expr)++;
  }
  if (**expr == 'u' || **expr == 'U' || **expr == 'l' || **expr == 'L') (*expr)++;
  FEXIT(result, *expr);
  return result;
}

result_t parse_char_constant(const char **expr) {
  FENTRY(*expr);
  result_t result = 0;
  if (**expr == '\'') {
    (*expr)++;
    result = **expr;
    (*expr)++;
    if (**expr == '\'') (*expr)++;
  }
  FEXIT(result, *expr);
  return result;
}

#ifdef TESTMAIN
// Test the implementation
int main() {
  const char *test_expr1 = "-123&321";
  const char *test_expr2 = "((1+-2))/0";
  const char *test_expr3 = "((5*3+2)>10)?(8|2):(4&1)";
  const char *test_expr4 = "1+2*3<4||5&6==7";

  result_t result1 = evaluate_expression(test_expr1);
  result_t result2 = evaluate_expression(test_expr2);
  result_t result3 = evaluate_expression(test_expr3);
  result_t result4 = evaluate_expression(test_expr4);

  printf("Result of test expression 1: %ld, %ld\n", result1, -123L&321L);  // Expected: 100
  printf("Result of test expression 2: %ld\n", result2);  // Expected: -1
  printf("Result of test expression 3: %ld\n", result3);  // Expected: 10
  printf("Result of test expression 4: %ld\n", result4);  // Expected: 1

  return 0;
}
#endif
