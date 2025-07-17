# Macro Handling Documentation for stcpp

## Overview

The stcpp (super tiny cpp) C preprocessor implements a comprehensive macro handling system that supports both simple macros and functional macros with parameters. The macro system is implemented primarily in `src/macro.c` and `src/macro.h`, with integration into the command line processing in `src/cmdline.c`.

## Core Data Structures

### Macro Structure
```c
typedef struct macro {
    struct macro *next;    // Pointer to the next macro in the linked list
    char *name;            // Name of the macro
    MacroParam *param;     // Pointer to the list of parameters (NULL for simple macros)
    char *replace;         // Replacement text of the macro
} Macro;
```

### MacroParam Structure
```c
typedef struct macroparam {
    struct macroparam *next; // Pointer to the next parameter
    char *name;              // Name of the parameter
} MacroParam;
```

## Macro Types

### 1. Simple Macros
- **Definition**: `#define MACRO_NAME replacement_text`
- **Example**: `#define PI 3.14159`
- **Characteristics**: 
  - No parameters
  - Direct text replacement
  - `param` field is NULL

### 2. Functional Macros
- **Definition**: `#define MACRO_NAME(param1, param2, ...) replacement_text`
- **Example**: `#define MAX(a, b) ((a) > (b) ? (a) : (b))`
- **Characteristics**:
  - Has parameter list
  - Parameters are replaced in the replacement text
  - Requires parentheses when invoked

### 3. Functional Macros Without Parameters
- **Definition**: `#define MACRO_NAME() replacement_text`
- **Example**: `#define EMPTY() `
- **Characteristics**:
  - Has empty parameter list
  - Special case with a single `MacroParam` node with `name = NULL`

## Core Functions

### 1. Macro Management Functions

#### `int addMacro(char *buf)`
**Purpose**: Parses and adds a new macro definition to the macro list.

**Process**:
1. Extracts macro name (must be valid identifier)
2. Checks for parameter list (if '(' follows name)
3. Parses parameters (comma-separated valid identifiers)
4. Extracts replacement text
5. Creates and links new `Macro` node

**Parameters**: Buffer containing the macro definition (without `#define`)

**Returns**: 
- `0` on success
- `-1` on error (invalid syntax, memory allocation failure)

**Example Usage**:
```c
addMacro("PI 3.14159");           // Simple macro
addMacro("MAX(a, b) ((a) > (b) ? (a) : (b))"); // Functional macro
```

#### `int deleteMacro(char *name)`
**Purpose**: Removes a macro from the macro list and frees its memory.

**Process**:
1. Searches for macro by name
2. Unlinks from linked list
3. Frees all associated memory (name, replacement text, parameters)

**Returns**:
- `0` on success
- `-1` if macro not found

#### `Macro *findMacro(char *start, char *end)`
**Purpose**: Searches for a macro by name in the macro list.

**Parameters**:
- `start`: Pointer to start of macro name
- `end`: Pointer to end of macro name

**Returns**: Pointer to `Macro` structure or `NULL` if not found

#### `int isdefinedMacro(char *start, char *end)`
**Purpose**: Checks if a macro is defined (used by `defined()` operator).

**Returns**:
- `1` if macro is defined
- `0` if not defined

### 2. Macro Processing Functions

#### `int processBuffer(char *buf, int len, int ifclausemode)`
**Purpose**: Main entry point for processing a buffer to expand macros.

**Process**:
1. Scans buffer for identifiers
2. Calls `processMacro()` for each potential macro
3. Skips strings, numbers, and other non-macro content
4. Handles special `ifclausemode` for conditional compilation

**Parameters**:
- `buf`: Buffer to process
- `len`: Length of buffer
- `ifclausemode`: Special mode for `#if` expressions

**Returns**:
- `0` on success
- Negative value on error

#### `int processMacro(char *buf, int len, int ifclausemode)`
**Purpose**: Processes a single macro invocation and performs replacement.

**Process**:
1. Identifies macro name
2. Looks up macro in macro list
3. For functional macros:
   - Parses arguments
   - Validates argument count
   - Creates temporary macro list for parameter replacement
4. Performs text replacement
5. Recursively processes parameters for nested macro expansion

**Special Cases**:
- Handles undefined macros in `ifclausemode` by replacing with "0"
- Validates parameter count for functional macros
- Supports nested macro expansion in parameters

### 3. Utility Functions

#### `char *skipSpaces(char *buf, char *end)`
Advances pointer past whitespace characters.

#### `char *skipString(char *buf, char *end)`
Advances pointer past string literals (handles escaped quotes).

#### `char *skipNumber(char *buf, char *end)`
Advances pointer past numeric literals.

#### `char *skipExpression(char *buf, char *end)`
Advances pointer past parenthesized expressions (handles nesting).

#### `int isIdent(char c, int idx)`
Checks if character is valid for identifiers:
- First character: letter or underscore
- Subsequent characters: letter, digit, or underscore

#### `char *replaceBuf(char *start, char *buf, char *end, char *replace)`
Replaces text in buffer with replacement string.

## Integration with Command Line Processing

### `#define` Directive
Handled in `cmdline.c`:
```c
case DEFINE:
    DPRINT("Define: %s\n", buf + 1);
    addMacro(buf + 1);
    break;
```

### `#undef` Directive
Handled in `cmdline.c`:
```c
case UNDEF:
    DPRINT("Undef: %s\n", buf + 1);
    deleteMacro(buf + 1);
    break;
```

### `defined()` Operator
Special handling in conditional expressions:
- Replaces `defined(MACRO)` with "1" if macro exists, "0" otherwise
- Used in `#if`, `#ifdef`, `#ifndef` expressions

## Macro Expansion Algorithm

### 1. Text Processing Flow
```
Input Buffer → processBuffer() → processMacro() → Text Replacement
```

### 2. Simple Macro Expansion
1. Identify macro name
2. Look up in macro list
3. Replace macro name with replacement text
4. Continue processing

### 3. Functional Macro Expansion
1. Identify macro name and opening parenthesis
2. Parse arguments (comma-separated, respecting nested parentheses)
3. Validate argument count against parameter count
4. Create temporary parameter macro list
5. Replace macro invocation with replacement text
6. Within replacement text, replace parameter names with argument values
7. Process result for nested macro expansions

### 4. Parameter Replacement
For functional macros, parameters are replaced using a temporary macro list:
```c
// Create temporary macros for each parameter
parammacro->name = param->name;      // Parameter name
parammacro->replace = argument_text; // Argument value
```

## Error Handling

### Common Error Conditions
1. **Invalid macro name**: Not a valid identifier
2. **Missing parentheses**: Functional macro called without parentheses
3. **Parameter count mismatch**: Wrong number of arguments
4. **Buffer overflow**: Replacement text too large for buffer
5. **Memory allocation failure**: Out of memory

### Error Reporting
- Returns `-1` on errors
- Debug messages via `DPRINT()` macros
- Some error conditions print to stderr

## Memory Management

### Allocation
- Macros: `malloc(sizeof(Macro))`
- Parameters: `malloc(sizeof(MacroParam))`
- Strings: `strdup()` for names and replacement text

### Deallocation
- `deleteMacro()` frees all associated memory
- Parameter lists are freed recursively
- Temporary parameter macros are freed after use

## Limitations and Known Issues

### Current Limitations
1. **No `##` (token pasting) operator**: Mentioned in code but not fully implemented
2. **No `#` (stringification) operator**: Not implemented
3. **Limited nested expansion**: Some edge cases may not be handled
4. **Buffer size constraints**: Fixed buffer sizes may cause overflow

### Future Improvements
- Token pasting (`##`) operator support
- Stringification (`#`) operator support
- Better error messages with line numbers
- Dynamic buffer sizing
- Optimization for large macro lists

## Usage Examples

### Simple Macros
```c
#define PI 3.14159
#define VERSION "1.0"
#define DEBUG 1

// Usage
double radius = PI * 2;  // Expands to: double radius = 3.14159 * 2;
```

### Functional Macros
```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define CONCAT(a, b) a ## b  // Note: Token pasting not fully supported

// Usage
int max_val = MAX(10, 20);        // Expands to: int max_val = ((10) > (20) ? (10) : (20));
int sq = SQUARE(5);               // Expands to: int sq = ((5) * (5));
```

### Conditional Compilation
```c
#define FEATURE_ENABLED 1

#if defined(FEATURE_ENABLED) && FEATURE_ENABLED
    // Code when feature is enabled
#endif
```

## Debugging and Diagnostics

### Debug Output
Enable debug output by commenting out `#define NDEBUG` in `macro.c`:
```c
// #define NDEBUG  // Comment out to enable debug output
```

### Available Debug Functions
- `printMacroList()`: Prints all defined macros
- `DPRINT()` statements throughout the code provide detailed tracing

This macro handling system provides a solid foundation for C preprocessing, supporting the most common macro patterns while maintaining simplicity and efficiency.
