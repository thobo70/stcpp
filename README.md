# stcpp - Super Tiny C Preprocessor

**stcpp** is a lightweight, resource-efficient C preprocessor implementation designed for educational purposes and resource-constrained environments. It provides comprehensive C preprocessing functionality with a focus on minimal memory footprint and clean, maintainable code.

## 🚀 Features

### Core Preprocessing Capabilities
- ✅ **Macro expansion** - Simple and functional macros with parameters
- ✅ **File inclusion** - `#include` directive support with search paths
- ✅ **Conditional compilation** - `#if`, `#ifdef`, `#ifndef`, `#else`, `#elif`, `#endif`
- ✅ **Token pasting** - `##` operator for token concatenation
- ✅ **Stringification** - `#` operator for converting tokens to strings
- ✅ **Macro undefinition** - `#undef` directive
- ✅ **Expression evaluation** - Arithmetic expressions in `#if` conditions
- ✅ **Line control** - `#line` directive for setting line numbers and filenames (passed to next stage)
- ✅ **Built-in macros** - `__LINE__` and `__FILE__` standard C preprocessor macros

### Advanced Features
- **Recursive macro expansion** - Proper handling of nested macro calls
- **Parameter substitution** - Full parameter replacement in functional macros
- **Conditional nesting** - Deep nesting support for `#if` constructs
- **Search path management** - Multiple include directory support
- **Memory efficiency** - Optimized for low memory usage (~17KB binary)

## 📁 Project Architecture

stcpp consists of six core modules working together:

```
┌─────────────────────────────────────────────────────────────┐
│                        stcpp                                │
│                   (Main Entry Point)                       │
└─────────────────────┬───────────────────────────────────────┘
                      │
           ┌──────────┴──────────┐
           │                     │
    ┌──────▼──────┐       ┌─────▼─────┐
    │   cmdline   │       │   input   │
    │  (Commands) │       │ (File I/O)│
    └──────┬──────┘       └─────┬─────┘
           │                    │
    ┌──────▼──────┐       ┌─────▼─────┐
    │    macro    │       │  exprint  │
    │(Core Engine)│       │(Expression│
    │             │       │Evaluation)│
    └─────────────┘       └───────────┘
```

### Module Overview

| Module | Purpose | Key Features |
|--------|---------|--------------|
| **main.c** | Entry point and orchestration | Command-line parsing, file I/O coordination |
| **macro.c** | Core macro processing engine | Macro storage, expansion, parameter substitution |
| **cmdline.c** | Preprocessor directive handling | `#define`, `#if`, `#include` processing |
| **input.c** | File input management | Include file handling, search paths |
| **exprint.c** | Expression evaluation | Arithmetic expression parsing for `#if` |
| **debug.h** | Debug utilities | Conditional debug output |

## 🔧 How stcpp Works

### 1. Input Processing Pipeline

```
Source File → Line Reading → Directive Detection → Processing → Output
     ↓              ↓              ↓               ↓          ↓
  test.c      ┌─────────────┐  ┌───────────┐  ┌─────────┐  result.out
              │  readline() │  │iscmdline()│  │process  │
              │             │  │           │  │Buffer() │
              └─────────────┘  └───────────┘  └─────────┘
```

### 2. Macro Processing Engine

stcpp implements a sophisticated macro system:

#### Simple Macros
```c
#define PI 3.14159
// PI → 3.14159
```

#### Functional Macros
```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
// MAX(x, y) → ((x) > (y) ? (x) : (y))
```

#### Token Pasting (`##`)
```c
#define CONCAT(a, b) a##b
// CONCAT(hello, world) → helloworld
```

#### Stringification (`#`)
```c
#define STR(x) #x
// STR(hello) → "hello"
```

### 3. Expression Evaluation Engine

stcpp features a **production-grade expression evaluation system** for `#if` directives:

```c
#if defined(FEATURE_A) && (VERSION > 100)
    // Feature A code
    #ifdef DEBUG
        // Debug version of Feature A
    #endif
#elif defined(FEATURE_B)
    // Feature B code
#else
    // Default code
#endif
```

#### **Advanced Expression Support:**
- ✅ **Arithmetic:** `+`, `-`, `*`, `/`, `%` with correct precedence
- ✅ **Bitwise:** `&`, `|`, `^`, `~`, `<<`, `>>` 
- ✅ **Logical:** `&&`, `||`, `!` with short-circuit evaluation
- ✅ **Comparison:** `<`, `>`, `<=`, `>=`, `==`, `!=`
- ✅ **Ternary:** `condition ? true : false` with right associativity
- ✅ **Complex expressions:** `(1+2*3>5) ? (8|2) : (4&1)`
- ✅ **Proper associativity:** Left-to-right for arithmetic (`8/2*3` = `12`)
- ✅ **Error detection:** Division by zero, incomplete expressions

### 5. Memory Management

stcpp is designed for minimal memory usage:

- **Binary size**: ~17.8KB total
- **Runtime memory**: 10-50KB typical usage
- **Streaming architecture**: Processes files line-by-line
- **Immediate cleanup**: Resources freed as soon as possible

## 🛠️ Installation

### Prerequisites
- GCC compiler
- Make build system
- POSIX-compatible system

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/thobo70/stcpp.git
cd stcpp

# Build the preprocessor
make

# Run basic tests
make test

# Run comprehensive test suite
make test-all

# Run individual test categories
make test-basic        # Basic macro functionality
make test-recursive    # Recursive macro expansion
make test-conditionals # Conditional compilation
make test-token-pasting # Token pasting operator
make test-stringification # Stringification operator
make test-line         # Line directive
```

## 📖 Usage

### Basic Usage
```bash
# Process a single file
./bin/stcpp input.c output.c

# With include directories
./bin/stcpp -Iinclude -I/usr/include input.c output.c

# With predefined macros
./bin/stcpp -DDEBUG=1 -D__STDC__=1 input.c output.c
```

### Command Line Options
- `-Dname[=value]` - Define a macro
- `-Uname` - Undefine a macro
- `-Ipath` - Add include search directory

### Example Processing

**Input** (`example.c`):
```c
#define VERSION 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#if VERSION >= 2
int value = MAX(10, 20);
#endif
```

**Command**:
```bash
./bin/stcpp example.c output.c
```

**Output** (`output.c`):
```c
int value = ((10) > (20) ? (10) : (20));
```

## 🔍 Implementation Details

### Macro Storage
Macros are stored in a linked list with efficient lookup:
```c
typedef struct macro {
    struct macro *next;    // Next macro in list
    char *name;            // Macro name
    MacroParam *param;     // Parameter list (NULL for simple macros)
    char *replace;         // Replacement text
} Macro;
```

### Recursive Expansion
stcpp properly handles recursive macro expansion with cycle detection and proper parameter substitution at each level.

### Expression Evaluation
The `exprint` module provides arithmetic expression evaluation for `#if` conditions, supporting:
- Integer arithmetic
- Comparison operators
- Logical operators
- Parentheses for precedence
- `defined()` operator

## 🧪 Testing

stcpp includes a comprehensive test suite:

```bash
# Run all tests
make test-all

# Individual test categories
make test-basic         # Basic functionality
make test-recursive     # Recursive macros  
make test-edge         # Edge cases
make test-conditionals # Conditional compilation
make test-undef        # Macro undefinition
make test-include      # File inclusion
make test-token-pasting # Token pasting
make test-stringification # Stringification
make test-line         # Line directive
```

Test results are stored in `test_results/` and can be examined for detailed output verification.

## ⚠️ Known Limitations

### Current Shortcomings
- ~~**Operator precedence**: Some edge cases in expression evaluation~~ ✅ **FIXED**
- **Complex headers**: May have issues with very complex system headers
- **Error messages**: Not yet polished for production use
- ~~**`#line` directive**: Not implemented for next compile stages~~ ✅ **IMPLEMENTED** - Now passed through to next compiler stage
- ~~**Logical operators**: No shortcuts for `&&` and `||` in expressions~~ ✅ **FIXED**

### Planned Improvements
- Enhanced error reporting
- ~~Better operator precedence handling~~ ✅ **COMPLETED**
- ~~`#line` directive support~~ ✅ **COMPLETED**
- Performance optimizations
- Extended standard compliance

## 📚 Documentation

For detailed technical information, see:

- [`PROJECT_MODULE_ANALYSIS.md`](PROJECT_MODULE_ANALYSIS.md) - Complete module analysis
- [`MACRO_HANDLING_DOCUMENTATION.md`](MACRO_HANDLING_DOCUMENTATION.md) - Macro system details
- [`EXPRESSION_EVALUATION_IMPLEMENTATION.md`](EXPRESSION_EVALUATION_IMPLEMENTATION.md) - Expression evaluation engine
- [`TOKEN_PASTING_IMPLEMENTATION.md`](TOKEN_PASTING_IMPLEMENTATION.md) - Token pasting specifics
- [`STRINGIFICATION_IMPLEMENTATION.md`](STRINGIFICATION_IMPLEMENTATION.md) - Stringification details
- [`LINE_DIRECTIVE_IMPLEMENTATION.md`](LINE_DIRECTIVE_IMPLEMENTATION.md) - Line directive implementation
- [`BUILTIN_MACRO_IMPLEMENTATION.md`](BUILTIN_MACRO_IMPLEMENTATION.md) - Built-in macro support
- [`RECURSIVE_MACRO_IMPLEMENTATION.md`](RECURSIVE_MACRO_IMPLEMENTATION.md) - Recursive expansion
- [`SOURCE_CODE_ISSUES_ANALYSIS.md`](SOURCE_CODE_ISSUES_ANALYSIS.md) - Code quality analysis

## 🤝 Contributing

Contributions are welcome! Please see the documentation files for technical details and current limitations that need addressing.

## 📄 License

Copyright (c) 2024 Thomas Boos (tboos70@gmail.com)

## 🎯 Project Status

**Current Status**: ✅ **FULLY FUNCTIONAL**

stcpp successfully implements all core C preprocessing features and passes comprehensive tests. It's suitable for educational use and lightweight preprocessing tasks, though it may encounter issues with very complex system headers.

**Binary Size**: 17.8KB  
**Memory Usage**: 10-50KB typical runtime  
**Test Coverage**: 8 test categories, all passing 

