/**
 * @file exprint.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Expression parser for arithmetic and logical operations.
 * @version 0.1
 * @date 2024-08-31
 * 
 * @copyright Copyright (c) 2024
 *
 * This file contains functions for parsing and evaluating expressions
 * involving integer constants, character constants, arithmetic operators,
 * bitwise operations, shifts, comparisons, and logical operations.
 * 
 * Operator precedence:
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

/**
 * @brief Sets the error state.
 * 
 * This function sets the global error state to the specified error code
 * and returns 0 to indicate an error.
 * 
 * @param error_code The error code to set.
 * @return Always returns 0 to indicate an error.
 */
static result_t set_error(int error_code) {
    expr_error = error_code;
    return 0;  // Indicate an error
}

/**
 * @brief Evaluates an expression.
 * 
 * This function initializes the error state and starts the parsing process
 * by calling parse_ternary.
 * 
 * @param expr The expression to evaluate.
 * @return The result of the evaluation.
 */
result_t evaluate_expression(const char *expr) {
    expr_error = 0;
    return parse_ternary(&expr);
}

/**
 * @brief Parses a ternary expression.
 * 
 * This function parses a ternary expression of the form `condition ? true_expr : false_expr`.
 * It first parses the condition using parse_logical_or, then parses the true and false expressions
 * if the condition is met.
 * 
 * @param expr The expression to parse.
 * @return The result of the ternary expression.
 */
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
        } else {
            result = set_error(EE_MISSINGCOLON);
        }
    }
    FEXIT(result, *expr);
    return result;
}

/**
 * @brief Parses a logical OR expression.
 * 
 * This function parses a logical OR expression of the form `expr1 || expr2`.
 * It first parses the left-hand side using parse_logical_and, then parses the right-hand side
 * if the logical OR operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the logical OR expression.
 */
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

/**
 * @brief Parses a logical AND expression.
 * 
 * This function parses a logical AND expression of the form `expr1 && expr2`.
 * It first parses the left-hand side using parse_bitwise_or, then parses the right-hand side
 * if the logical AND operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the logical AND expression.
 */
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

/**
 * @brief Parses a bitwise OR expression.
 * 
 * This function parses a bitwise OR expression of the form `expr1 | expr2`.
 * It first parses the left-hand side using parse_bitwise_xor, then parses the right-hand side
 * if the bitwise OR operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the bitwise OR expression.
 */
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

/**
 * @brief Parses a bitwise XOR expression.
 * 
 * This function parses a bitwise XOR expression of the form `expr1 ^ expr2`.
 * It first parses the left-hand side using parse_bitwise_and, then parses the right-hand side
 * if the bitwise XOR operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the bitwise XOR expression.
 */
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

/**
 * @brief Parses a bitwise AND expression.
 * 
 * This function parses a bitwise AND expression of the form `expr1 & expr2`.
 * It first parses the left-hand side using parse_equality, then parses the right-hand side
 * if the bitwise AND operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the bitwise AND expression.
 */
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

/**
 * @brief Parses an equality expression.
 * 
 * This function parses an equality expression of the form `expr1 == expr2` or `expr1 != expr2`.
 * It first parses the left-hand side using parse_relational, then parses the right-hand side
 * if the equality operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the equality expression.
 */
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

/**
 * @brief Parses a relational expression.
 * 
 * This function parses a relational expression of the form `expr1 < expr2`, `expr1 <= expr2`,
 * `expr1 > expr2`, or `expr1 >= expr2`. It first parses the left-hand side using parse_shift,
 * then parses the right-hand side if the relational operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the relational expression.
 */
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

/**
 * @brief Parses a shift expression.
 * 
 * This function parses a shift expression of the form `expr1 << expr2` or `expr1 >> expr2`.
 * It first parses the left-hand side using parse_additive, then parses the right-hand side
 * if the shift operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the shift expression.
 */
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

/**
 * @brief Parses an additive expression.
 * 
 * This function parses an additive expression of the form `expr1 + expr2` or `expr1 - expr2`.
 * It first parses the left-hand side using parse_multiplicative, then parses the right-hand side
 * if the additive operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the additive expression.
 */
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

/**
 * @brief Parses a multiplicative expression.
 * 
 * This function parses a multiplicative expression of the form `expr1 * expr2`, `expr1 / expr2`,
 * or `expr1 % expr2`. It first parses the left-hand side using parse_unary, then parses the
 * right-hand side if the multiplicative operator is present.
 * 
 * @param expr The expression to parse.
 * @return The result of the multiplicative expression.
 */
result_t parse_multiplicative(const char **expr) {
    FENTRY(*expr);
    result_t result = parse_unary(expr);
    while (**expr == '*' || **expr == '/' || **expr == '%') {
        char op = **expr;
        (*expr)++;
        result_t rhs = parse_ternary(expr);
        if (op == '*') result *= rhs;
        else if (op == '%') result %= rhs;
        else if (op == '/') {
            if (rhs == 0) {
                result = set_error(EE_DIVBYZERO);
                break;
            }
            result /= rhs;
        }
    }
    FEXIT(result, *expr);
    return result;
}

/**
 * @brief Parses a unary expression.
 * 
 * This function parses a unary expression of the form `+expr`, `-expr`, `!expr`, or `~expr`.
 * It first parses the unary operator, then parses the operand using parse_primary.
 * 
 * @param expr The expression to parse.
 * @return The result of the unary expression.
 */
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

/**
 * @brief Parses a primary expression.
 * 
 * This function parses a primary expression, which can be a parenthesized expression,
 * a number, or a character constant. It first skips any leading whitespace, then
 * determines the type of primary expression and parses it accordingly.
 * 
 * @param expr The expression to parse.
 * @return The result of the primary expression.
 */
result_t parse_primary(const char **expr) {
    FENTRY(*expr);
    result_t result;
    while (isspace(**expr)) (*expr)++;
    if (**expr == '(') {
        (*expr)++;
        result = parse_ternary(expr);
        if (**expr == ')') {
            (*expr)++;
        } else {
            result = set_error(EE_MISSINGPAREN);
        }
    } else if (isdigit(**expr)) {
        result = parse_number(expr);
    } else if (**expr == '\'') {
        result = parse_char_constant(expr);
    } else {
        result = set_error(EE_UNEXPECTEDCHAR);
    }
    FEXIT(result, *expr);
    return result;
}

/**
 * @brief Parses a number.
 * 
 * This function parses a number, which can be in decimal, hexadecimal, binary, or octal format.
 * It first determines the base of the number, then parses the digits accordingly.
 * 
 * @param expr The expression to parse.
 * @return The result of the number.
 */
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
        if (digit >= base) {
            result = set_error(EE_INVALDIGIT);
            break;
        }
        result = result * base + digit;
        (*expr)++;
    }
    if (**expr == 'u' || **expr == 'U' || **expr == 'l' || **expr == 'L') (*expr)++;
    FEXIT(result, *expr);
    return result;
}

/**
 * @brief Parses a character constant.
 * 
 * This function parses a character constant of the form `'c'`. It first skips the opening
 * single quote, then reads the character, and finally skips the closing single quote.
 * 
 * @param expr The expression to parse.
 * @return The result of the character constant.
 */
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
/**
 * @brief Main function for testing the expression parser.
 * 
 * This function tests the expression parser with various test expressions and prints the results.
 * 
 * @return 0 on success, non-zero on failure.
 */
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
