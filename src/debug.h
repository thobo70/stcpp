/**
 * @file debug.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DEBUG_H  // NOLINT
#define DEBUG_H

#include <stdio.h>
#include <assert.h>

#ifdef NDEBUG
#define DPRINT(fmt, ...)  do { } while (0)
#define DPRINTERR(fmt, ...)  do { } while (0)
#define D(statement)
#else
#define DPRINT(fmt, ...)  do { fprintf(stderr, fmt __VA_OPT__( , ) __VA_ARGS__); } while (0)
#define DPRINTERR(fmt, ...)  \
  do { fprintf(stderr, "%s[%s]: ", __FILE__, __LINE__); fprintf(stderr, fmt __VA_OPT__( , ) __VA_ARGS__); } while (0)
#define D(statement)  statement;
#endif

#endif  // DEBUG_H  // NOLINT
