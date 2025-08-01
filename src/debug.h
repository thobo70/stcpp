/**
 * @file debug.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Debug macros and utilities for development and troubleshooting.
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This header file provides debugging macros that can be conditionally compiled
 * based on the NDEBUG preprocessor definition. When NDEBUG is defined (release mode),
 * debug macros are compiled away to no-ops for optimal performance.
 */

#ifndef DEBUG_H  // NOLINT
#define DEBUG_H

#include <stdio.h>
#include <assert.h>

#ifdef NDEBUG
/**
 * @brief Debug print macro (disabled in release builds).
 * 
 * In release builds (when NDEBUG is defined), this macro expands to nothing,
 * eliminating any performance overhead from debug output.
 * 
 * @param fmt Printf-style format string.
 * @param ... Variable arguments for format string.
 */
#define DPRINT(fmt, ...)  do { } while (0)

/**
 * @brief Debug error print macro (disabled in release builds).
 * 
 * In release builds (when NDEBUG is defined), this macro expands to nothing.
 * 
 * @param fmt Printf-style format string.
 * @param ... Variable arguments for format string.
 */
#define DPRINTERR(fmt, ...)  do { } while (0)

/**
 * @brief Debug statement macro (disabled in release builds).
 * 
 * In release builds, this macro expands to nothing, allowing debug-only
 * code to be conditionally compiled away.
 * 
 * @param statement Statement to execute only in debug builds.
 */
#define D(statement)
#else
/**
 * @brief Debug print macro (enabled in debug builds).
 * 
 * In debug builds (when NDEBUG is not defined), this macro prints debug
 * information to stderr using printf-style formatting.
 * 
 * @param fmt Printf-style format string.
 * @param ... Variable arguments for format string.
 */
#define DPRINT(fmt, ...)  do { fprintf(stderr, fmt __VA_OPT__( , ) __VA_ARGS__); } while (0)

/**
 * @brief Debug error print macro with file and line information.
 * 
 * In debug builds, this macro prints error information to stderr along with
 * the source file name and line number where the error occurred.
 * 
 * @param fmt Printf-style format string.
 * @param ... Variable arguments for format string.
 */
#define DPRINTERR(fmt, ...)  \
  do { fprintf(stderr, "%s[%s]: ", __FILE__, __LINE__); fprintf(stderr, fmt __VA_OPT__( , ) __VA_ARGS__); } while (0)

/**
 * @brief Debug statement macro (enabled in debug builds).
 * 
 * In debug builds, this macro executes the provided statement, allowing
 * debug-only code to be conditionally compiled.
 * 
 * @param statement Statement to execute only in debug builds.
 */
#define D(statement)  statement;
#endif

#endif  // DEBUG_H  // NOLINT
