# STCPP User Guide

**Super Tiny C Preprocessor - Complete User Manual**

## Table of Contents

1. [Overview](#overview)
2. [Installation and Usage](#installation-and-usage)
3. [Command-Line Options](#command-line-options)
4. [Macro Definitions](#macro-definitions)
5. [Conditional Compilation](#conditional-compilation)
6. [Advanced Features](#advanced-features)
7. [Built-in Macros](#built-in-macros)
8. [Include Files](#include-files)
9. [Line Directives](#line-directives)
10. [Examples](#examples)
11. [Tips and Best Practices](#tips-and-best-practices)

---

## Overview

STCPP is a lightweight, fully-featured C preprocessor that handles all standard preprocessing operations. It supports macro definitions, conditional compilation, file inclusion, and advanced features like token pasting and stringification.

**Key Features:**
- Complete macro expansion with recursion support
- Token pasting (`##`) and stringification (`#`)  
- Conditional compilation (`#if`, `#ifdef`, `#ifndef`)
- Command-line macro definitions and undefinitions
- Include file processing with `CPATH` environment variable support
- Built-in macros (`__LINE__`, `__FILE__`)
- Standard C preprocessor compatibility

---

## Installation and Usage

### Building STCPP

```bash
# Build the preprocessor
make

# Clean build artifacts
make clean
```

### Basic Usage

```bash
# Basic preprocessing
stcpp input.c output.c

# Using stdin/stdout
echo '#define TEST 42' | stcpp - -

# With command-line options
stcpp -DDEBUG=1 -Iinclude input.c output.c
```

**Syntax:**
```
stcpp [-Dname[=value]] [-Uname] [-Ipath] [-h] infile outfile
```

---

## Command-Line Options

### `-D` Define Macros

Define macros from the command line:

```bash
# Define macro as 1
stcpp -DDEBUG input.c output.c

# Define macro with value
stcpp -DVERSION=2 input.c output.c

# Define macro with string value
stcpp -DMESSAGE='"Hello World"' input.c output.c

# Multiple definitions
stcpp -DDEBUG=1 -DVERSION=2 -DPLATFORM='"Linux"' input.c output.c
```

**Examples:**
- `-DDEBUG` → `#define DEBUG 1`
- `-DMAX_SIZE=1024` → `#define MAX_SIZE 1024`
- `-DVERSION='"1.2.3"'` → `#define VERSION "1.2.3"`

### `-U` Undefine Macros

Prevent macros from being defined, even if they appear in the source:

```bash
# Undefine a macro
stcpp -UOLD_FEATURE input.c output.c

# Multiple undefinitions
stcpp -UDEBUG -UTEST_MODE input.c output.c
```

**Note:** `-U` creates a "banned" list - macros specified with `-U` cannot be defined later in the source code.

### `-I` Include Paths

Add directories to the include search path:

```bash
# Single include path
stcpp -Iinclude input.c output.c

# Multiple include paths
stcpp -Iinclude -I../common -I/usr/local/include input.c output.c
```

### `CPATH` Environment Variable

STCPP automatically reads the `CPATH` environment variable to set up include search paths. This provides a convenient way to configure global include directories without specifying `-I` options repeatedly.

```bash
# Set CPATH environment variable
export CPATH="/usr/local/include:/opt/include:/home/user/includes"

# Now STCPP will search these directories automatically
stcpp input.c output.c

# You can still add more paths with -I (these are searched first)
stcpp -Iproject/include input.c output.c
```

**Search Order:**
1. Current directory
2. Directories specified with `-I` options (in order given)
3. Directories from `CPATH` environment variable (in order listed)

**CPATH Format:**
- Unix/Linux: Colon-separated paths: `/path1:/path2:/path3`
- The paths are processed in the order they appear in CPATH

**Examples:**
```bash
# Development setup
export CPATH="/usr/include:/usr/local/include:/opt/dev/include"
stcpp -Isrc/include input.c output.c

# Will search in this order:
# 1. ./
# 2. src/include/
# 3. /usr/include/
# 4. /usr/local/include/
# 5. /opt/dev/include/
```

### `-h` Help

Display usage information and exit.

---

## Macro Definitions

### Simple Macros

```c
#define PI 3.14159
#define DEBUG 1
#define VERSION "1.0"
#define EMPTY

// Usage
double area = PI * radius * radius;  // → 3.14159 * radius * radius
int flag = DEBUG;                    // → 1
char *ver = VERSION;                 // → "1.0"
int test = EMPTY 42;                 // → 42
```

### Function-Like Macros

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define PRINT_VAR(name) printf(#name " = %d\n", name)

// Usage
int maximum = MAX(10, 20);           // → ((10) > (20) ? (10) : (20))
int result = SQUARE(5 + 3);          // → ((5 + 3) * (5 + 3))
PRINT_VAR(count);                    // → printf("count" " = %d\n", count)
```

### Macro Redefinition

```c
#define TEMP 100
int before = TEMP;                   // → 100

#undef TEMP
#define TEMP 200
int after = TEMP;                    // → 200
```

### Recursive Macro Expansion

```c
#define A 42
#define B A
#define C B
#define D C

int value = D;                       // → 42

// Function-like recursion
#define DOUBLE(x) ((x) * 2)
#define QUAD(x) DOUBLE(DOUBLE(x))
int result = QUAD(5);                // → (((5) * 2) * 2)
```

---

## Conditional Compilation

### `#if` / `#else` / `#endif`

```c
#define VERSION_MAJOR 2
#define VERSION_MINOR 1

#if VERSION_MAJOR > 1
    printf("Using version 2.x\n");
#else
    printf("Using version 1.x\n");
#endif

#if (VERSION_MAJOR == 2) && (VERSION_MINOR >= 1)
    // This code will be included
    printf("Version 2.1 or later\n");
#endif
```

### `#ifdef` / `#ifndef`

```c
#define DEBUG 1

#ifdef DEBUG
    printf("Debug mode enabled\n");  // This will be included
#endif

#ifndef RELEASE
    printf("Not a release build\n"); // This will be included
#endif
```

### `#if defined()`

```c
#define FEATURE_A 1

#if defined(FEATURE_A)
    printf("Feature A is available\n");
#endif

#if defined(FEATURE_A) && defined(FEATURE_B)
    printf("Both features available\n");
#endif

#if !defined(OLD_API)
    printf("Using new API\n");
#endif
```

---

## Advanced Features

### Token Pasting (`##`)

Concatenate tokens to create new identifiers:

```c
#define CONCAT(a, b) a ## b
#define MAKE_FUNC(name) void func_ ## name(void)
#define DECLARE_VAR(type, num) type var ## num

int CONCAT(hello, world) = 42;       // → int helloworld = 42;
MAKE_FUNC(init);                     // → void func_init(void)
DECLARE_VAR(int, 1) = 100;           // → int var1 = 100;

// Complex token pasting
#define TRIPLE(a, b, c) a ## b ## c
int TRIPLE(x, y, z) = 999;           // → int xyz = 999;
```

### Stringification (`#`)

Convert macro arguments to string literals:

```c
#define STRINGIFY(x) #x
#define PRINT_EXPR(expr) printf("%s = %d\n", #expr, (expr))
#define MAKE_STRING(name) char name##_str[] = #name

char *test = STRINGIFY(hello world);  // → "hello world"
PRINT_EXPR(2 + 3);                   // → printf("2 + 3" " = %d\n", (2 + 3))
MAKE_STRING(debug);                  // → char debug_str[] = "debug"
```

### Combined Operations

```c
#define LOG_VAR(name) printf(#name " = %d\n", name)
#define DECLARE_AND_INIT(type, name, val) type name = val; LOG_VAR(name)

int count = 10;
LOG_VAR(count);                      // → printf("count" " = %d\n", count)

DECLARE_AND_INIT(int, size, 1024);   // → int size = 1024; printf("size" " = %d\n", size)
```

### String Concatenation

```c
#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define STRINGIFY(x) #x
#define VERSION_STRING "Version " STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR)

const char *version = VERSION_STRING;
// → const char *version = "Version " "2" "." "1";
```

---

## Built-in Macros

### `__LINE__`

Expands to the current line number:

```c
printf("Error at line %d\n", __LINE__);  // → printf("Error at line %d\n", 15);
```

### `__FILE__`

Expands to the current filename (with quotes):

```c
printf("File: %s\n", __FILE__);          // → printf("File: %s\n", "input.c");
```

### Combined Usage

```c
#define DEBUG_PRINT(msg) printf("[%s:%d] %s\n", __FILE__, __LINE__, msg)

DEBUG_PRINT("Starting function");
// → printf("[%s:%d] %s\n", "input.c", 42, "Starting function");
```

---

## Include Files

### Basic Include

```c
#include "myheader.h"    // Search in current directory first
#include <stdio.h>       // Search in system directories
```

### Include Search Path

STCPP searches for include files in the following order:

1. **Current directory** (always searched first)
2. **`-I` directories** (command-line specified paths, in order given)
3. **`CPATH` directories** (environment variable paths, in order listed)

### Include with Search Paths

**Using command line:**
```bash
stcpp -Iinclude -I../common input.c output.c
```

**Using CPATH environment variable:**
```bash
export CPATH="/usr/include:/usr/local/include"
stcpp input.c output.c
```

**Combined usage:**
```bash
export CPATH="/usr/include:/usr/local/include"
stcpp -Iproject/include -I../shared input.c output.c
```

**Example include resolution:**
```c
#include "config.h"      // Searches: 
                         // 1. ./config.h
                         // 2. project/include/config.h  
                         // 3. ../shared/config.h
                         // 4. /usr/include/config.h
                         // 5. /usr/local/include/config.h
```

### Nested Includes

Header files can include other header files. STCPP handles nested inclusions automatically.

---

## Line Directives

### `#line` Directive

Set line number and optionally filename:

```c
#line 100
int test = __LINE__;                 // → int test = 100;

#line 200 "virtual.c"
printf("File: %s, Line: %d\n", __FILE__, __LINE__);
// → printf("File: %s, Line: %d\n", "virtual.c", 200);
```

### Passthrough Behavior

Line directives are passed through to the output for use by compilers and debuggers.

---

## Examples

### Complete Preprocessing Example

**Input file (example.c):**
```c
#define PI 3.14159
#define CIRCLE_AREA(r) (PI * (r) * (r))
#define STRINGIFY(x) #x
#define VERSION 2

#if VERSION >= 2
    #define FEATURE_ENABLED 1
#else
    #define FEATURE_ENABLED 0
#endif

#ifdef FEATURE_ENABLED
const char* status = "Feature is " STRINGIFY(FEATURE_ENABLED);
#endif

double area = CIRCLE_AREA(5.0);
int line = __LINE__;
```

**Command:**
```bash
stcpp -DDEBUG=1 example.c output.c
```

**Output (output.c):**
```c
const char* status = "Feature is " "1";
double area = (3.14159 * (5.0) * (5.0));
int line = 12;
```

### Complex Macro System

```c
#define DECLARE_GETTER_SETTER(type, name) \
    type get_##name(void) { return name; } \
    void set_##name(type value) { name = value; }

#define LOG_FUNCTION_ENTRY() \
    printf("Entering %s at %s:%d\n", __func__, __FILE__, __LINE__)

int global_count = 0;
DECLARE_GETTER_SETTER(int, global_count)
// Expands to:
// int get_global_count(void) { return global_count; }
// void set_global_count(int value) { global_count = value; }
```

### Conditional Feature Compilation

```c
#define FEATURE_LOGGING 1
#define FEATURE_DEBUG 0

#if FEATURE_LOGGING
    #define LOG(msg) printf("[LOG] %s\n", msg)
#else
    #define LOG(msg) /* disabled */
#endif

#if FEATURE_DEBUG
    #define DBG_PRINT(fmt, ...) printf("[DBG] " fmt "\n", __VA_ARGS__)
#else
    #define DBG_PRINT(fmt, ...) /* disabled */
#endif

// Usage
LOG("Application started");           // → printf("[LOG] %s\n", "Application started");
DBG_PRINT("Value: %d", 42);          // → /* disabled */
```

### Environment Variable Setup

**Setting up a development environment:**

```bash
# Set up include paths for your project
export CPATH="/usr/include:/usr/local/include:/opt/myproject/include"

# Create a simple header file
echo '#define PROJECT_VERSION "1.0"' > /opt/myproject/include/version.h

# Create input file that uses the header
cat > main.c << 'EOF'
#include "version.h"
#include <stdio.h>

int main() {
    printf("Project version: %s\n", PROJECT_VERSION);
    return 0;
}
EOF

# Preprocess (will automatically find version.h via CPATH)
stcpp main.c main_processed.c
```

**Result:**
```c
int main() {
    printf("Project version: %s\n", "1.0");
    return 0;
}
```

---

## Tips and Best Practices

### 1. Macro Naming Conventions

```c
// Use ALL_CAPS for constants
#define MAX_BUFFER_SIZE 1024
#define PI 3.14159

// Use descriptive names for function-like macros
#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

### 2. Parentheses in Macro Definitions

Always use parentheses around parameters and the entire expression:

```c
// Good
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Bad - can cause issues with operator precedence
#define SQUARE(x) x * x
#define MAX(a, b) a > b ? a : b
```

### 3. Multi-line Macros

Use backslash continuation for complex macros:

```c
#define SWAP(a, b, temp) \
    do { \
        temp = a; \
        a = b; \
        b = temp; \
    } while(0)
```

### 4. Avoiding Side Effects

Be careful with macros that evaluate parameters multiple times:

```c
// This macro evaluates 'a' and 'b' twice each
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Problematic usage:
int result = MAX(++i, ++j);  // i and j are incremented multiple times!
```

### 5. Using Command-Line Options and Environment Variables Effectively

```bash
# Development build with environment setup
export CPATH="/usr/include:/usr/local/include:/project/include"
stcpp -DDEBUG=1 -DVERBOSE=1 -Isrc/include src/main.c build/main_debug.c

# Release build  
stcpp -DNDEBUG -DOPTIMIZED=1 -Iinclude src/main.c build/main_release.c

# Testing build with specific include override
stcpp -DTEST_MODE=1 -UDEBUG -Itest/mocks -Iinclude src/main.c build/main_test.c
```

**Setting up team development environment:**
```bash
# In your project's setup script
export CPATH="/opt/company/include:/usr/local/include"
export PROJECT_INCLUDES="include:../common/include"

# Team members can then use:
stcpp -I${PROJECT_INCLUDES//:/ -I} src/main.c build/main.c
```

### 6. Organizing Large Macro Systems

```c
// config.h
#ifndef CONFIG_H
#define CONFIG_H

#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#if defined(DEBUG_BUILD)
    #define ENABLE_LOGGING 1
    #define ENABLE_ASSERTIONS 1
#else
    #define ENABLE_LOGGING 0
    #define ENABLE_ASSERTIONS 0
#endif

#endif // CONFIG_H
```

### 7. Error Handling

STCPP will return non-zero exit codes on errors. Check return values in scripts:

```bash
#!/bin/bash
if ! stcpp -DDEBUG=1 input.c output.c; then
    echo "Preprocessing failed!"
    exit 1
fi
echo "Preprocessing successful"
```

---

## Summary

STCPP is a complete C preprocessor implementation supporting:

- **Full macro system** with recursion and complex expansions
- **Advanced features** including token pasting and stringification  
- **Conditional compilation** with expression evaluation
- **Command-line macro management** with `-D` and `-U` options
- **Include file processing** with search path support
- **Built-in macros** for line numbers and filenames
- **Standard compatibility** with existing C preprocessing workflows

The preprocessor handles complex real-world scenarios while maintaining simplicity and reliability for everyday use.
