#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <assert.h>

#ifdef NDEBUG
#define DPRINT(fmt, ...)  do { } while (0)
#define DPRINTERR(fmt, ...)  do { } while (0)
#else
#define DPRINT(fmt, ...)  do { fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__); } while (0)
#define DPRINTERR(fmt, ...)  do { fprinf(stderr, "%s[%s]: ", __FILE__, __LINE__); fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__); } while (0)
#endif

#endif // DEBUG_H
