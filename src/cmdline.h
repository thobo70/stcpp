/**
 * @file cmdline.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Header file for preprocessor directive processing functionality.
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This header file contains enumerations and function declarations for processing
 * C preprocessor directives such as #include, #define, #if, #ifdef, #ifndef, etc.
 */

#ifndef _CMDLINE_H
#define _CMDLINE_H

/**
 * @enum cmdcondstate
 * @brief Enumeration for conditional compilation states.
 * 
 * Tracks the state of conditional compilation blocks to properly
 * handle #if/#else/#endif nesting and control flow.
 */
typedef enum cmdcondstate {
  COND_IF,   /**< Currently in #if or #elif block. */
  COND_ELSE  /**< Currently in #else block. */
} cmdcondstate_t;

/**
 * @brief Global variable tracking current conditional compilation state.
 * 
 * When 1, code is being processed normally. When 0, code is being
 * skipped due to failed conditional compilation tests.
 */
extern int condstate;

/**
 * @brief Checks if a line is a preprocessor directive.
 * 
 * Determines whether a line starts with '#' and is therefore
 * a preprocessor directive that needs special processing.
 * 
 * @param line The line to check.
 * @return 1 if line is a preprocessor directive, 0 otherwise.
 */
int iscmdline(char *line);

/**
 * @brief Processes a preprocessor directive line.
 * 
 * Parses and executes preprocessor directives including:
 * - #include: File inclusion
 * - #define: Macro definition
 * - #undef: Macro undefinition
 * - #if/#ifdef/#ifndef: Conditional compilation
 * - #else/#elif/#endif: Conditional compilation control
 * - #error: Error generation
 * - #pragma: Implementation-specific directives
 * - #line: Line number control
 * 
 * @param buf Buffer containing the preprocessor directive line.
 * @param size Size of the buffer.
 * @param outfile Output file for generating #line directives.
 * @return 0 on success, -1 on error.
 */
int processcmdline(char *buf, int size, FILE *outfile);


#endif
