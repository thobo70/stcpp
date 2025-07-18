# Built-in Macro Implementation Documentation

## Overview

This document describes the implementation of built-in preprocessor macros in stcpp, specifically `__LINE__` and `__FILE__`. These macros are part of the C standard and provide compile-time information about the current source location.

## Implementation Details

### Built-in Macro Support

The built-in macro functionality is implemented in `src/macro.c` with the following key components:

1. **getBuiltinMacro() Function**: Checks if a given identifier is a built-in macro and returns its expanded value
2. **Integration with Macro Expansion**: Built-in macros are checked when regular macro lookup fails
3. **Dynamic Content Generation**: Values are generated at expansion time based on current file and line context

### Supported Built-in Macros

#### `__LINE__`
- **Purpose**: Expands to the current line number as a decimal integer
- **Implementation**: Uses `getcurrentinstream()->linenumber` to get current line
- **Behavior**: Each occurrence expands to the line number where the macro appears

#### `__FILE__`
- **Purpose**: Expands to the current filename as a string literal
- **Implementation**: Uses `getcurrentinstream()->filename` with proper escaping
- **Behavior**: Expands to the name of the file being processed

### Technical Implementation

#### Function Signature
```c
char *getBuiltinMacro(char *start, char *end)
```

#### Implementation Strategy
1. **Identifier Matching**: Compare the token against known built-in macro names
2. **Static Buffer**: Use static buffers for return values to ensure persistence
3. **Dynamic Content**: Generate content based on current input stream state
4. **String Escaping**: Properly escape filenames for string literal format

#### Integration Points
- **processMacro()**: Modified to check built-in macros when `findMacro()` returns NULL
- **Expansion Priority**: Built-in macros are checked before treating unknown identifiers as undefined

### Code Flow

1. **Macro Lookup**: `processMacro()` calls `findMacro()` for regular macros
2. **Built-in Check**: If no regular macro found, calls `getBuiltinMacro()`
3. **Expansion**: If built-in macro found, uses `replaceBuf()` to substitute value
4. **Continuation**: If neither found, continues with normal undefined macro handling

### Testing

The built-in macro functionality is tested with:
- **Direct Usage**: `__LINE__` and `__FILE__` used directly in code
- **Macro Definition**: Built-in macros used within other macro definitions
- **Multiple Occurrences**: Testing that line numbers increment properly
- **File Handling**: Verifying correct filename expansion

Test file: `test/test_builtin_macros_proper.c`

### Standards Compliance

This implementation follows C standard requirements for:
- **`__LINE__`**: Integer literal representing current line number
- **`__FILE__`**: String literal containing source filename
- **Expansion Context**: Macros expand at the point of usage, not definition

### Future Extensions

The architecture supports adding additional built-in macros such as:
- `__DATE__`: Current date as string literal
- `__TIME__`: Current time as string literal
- `__STDC__`: Standard compliance indicator
- `__VERSION__`: Compiler version information

### Performance Considerations

- **Static Buffers**: Avoid dynamic allocation for better performance
- **String Operations**: Minimal string manipulation for efficiency
- **Integration Overhead**: Minimal impact on regular macro processing

## Files Modified

- `src/macro.c`: Added `getBuiltinMacro()` function and integration logic
- `src/macro.h`: Added function prototype for `getBuiltinMacro()`
- `test/test_builtin_macros_proper.c`: Comprehensive test cases
- `test/run_tests.sh`: Added built-in macro test to test suite

## Usage Examples

```c
// Line number expansion
int current_line = __LINE__;  // Expands to line number

// File name expansion
const char *current_file = __FILE__;  // Expands to "filename.c"

// In macro definitions
#define DEBUG_INFO __FILE__ ":" __LINE__
const char *debug = DEBUG_INFO;  // Combines file and line
```

The implementation provides full C standard compliance for these essential preprocessing features while maintaining the performance and simplicity of the existing macro system.
