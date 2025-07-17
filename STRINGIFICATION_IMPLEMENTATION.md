# Stringification (#) Implementation

## Overview

This document describes the implementation of the stringification operator (`#`) in the stcpp C preprocessor. The `#` operator converts macro arguments into string literals during macro expansion, which is essential for creating debugging macros, error messages, and other string-based functionality.

## Implementation Details

### Core Function: `stringifyParameter()`

The stringification functionality is implemented through the `stringifyParameter()` function in `src/macro.c`, which converts parameter values into properly quoted string literals.

**Function signature:**
```c
int stringifyParameter(const char *value, char *result, size_t result_size)
```

**Processing:**
1. Adds opening quote (`"`)
2. Escapes internal quotes and backslashes
3. Copies the parameter value
4. Adds closing quote (`"`)
5. Null-terminates the result

### Integration with Parameter Substitution

The stringification logic is integrated into the parameter substitution loop in `processMacro()`. When a `#` is encountered followed by a parameter name, the parameter value is stringified instead of directly substituted.

**Processing order:**
1. Scan for `#` operators during parameter substitution
2. Check if the token following `#` is a valid parameter name
3. If valid, stringify the parameter value
4. Replace `#parameter` with `"parameter_value"`
5. Continue with normal parameter substitution

### Algorithm Details

#### Token Recognition
- `#` must not be part of `##` (token pasting)
- Must be followed by a valid identifier (parameter name)
- Parameter name must exist in the macro's parameter list

#### String Creation
- Wraps parameter value in double quotes
- Escapes embedded quotes (`"` → `\"`) 
- Escapes backslashes (`\` → `\\`)
- Handles empty parameters (results in `""`)

#### Buffer Management
- Uses temporary buffer for string construction
- Safely replaces `#parameter` with stringified result
- Maintains buffer integrity during expansion

## Supported Use Cases

### Basic Stringification
```c
#define STR(x) #x
STR(hello) → "hello"
STR(123) → "123"
```

### Debug Macros
```c
#define DEBUG_PRINT(var) printf("var = " #var " = %d\n", var)
DEBUG_PRINT(count) → printf("var = " "count" " = %d\n", count)
```

### Message Generation
```c
#define MESSAGE(msg) "Error: " #msg
MESSAGE(File not found) → "Error: " "File not found"
```

### Expression Stringification
```c
#define STRINGIFY(expr) #expr
STRINGIFY(a + b * c) → "a + b * c"
```

### Combined with Token Pasting
```c
#define MAKE_STRING_VAR(name) char name##_str[] = #name
MAKE_STRING_VAR(test) → char test_str[] = "test"
```

## Technical Considerations

### Processing Order
1. Macro replacement text is inserted
2. Stringification (`#`) is processed during parameter substitution
3. Token pasting (`##`) is processed after parameter substitution
4. Recursive macro expansion continues

### String Escaping Rules
- **Quotes**: `"` becomes `\"`
- **Backslashes**: `\` becomes `\\`
- **Other characters**: Preserved as-is
- **Whitespace**: Preserved within the stringified content

### Buffer Safety
- Maximum string length: 512 characters
- Overflow protection with bounds checking
- Safe memory operations using standard library functions

### Error Handling
- Invalid parameter names are ignored
- Buffer overflow results in skipping the stringification
- Malformed `#` operators are left unchanged

## Limitations

### Current Limitations
1. **Buffer size**: Limited to 512 characters for stringified results
2. **Escape sequences**: Only handles quotes and backslashes
3. **Complex tokens**: Basic identifier-based parameter detection
4. **Error reporting**: Limited feedback for failures

### C Standard Compliance
- Follows C99/C11 stringification semantics
- Compatible with standard preprocessing behavior
- Handles most common use cases correctly

## Testing

### Test Coverage
The stringification implementation includes comprehensive tests:
- `test/test_stringification.c`: Full test suite
- Integration with `make test-all`
- Individual testing via `make test-stringification`

### Test Cases
```c
// Basic stringification
#define STR(x) #x
STR(hello) → "hello"

// Complex expressions  
STR(a + b) → "a + b"

// Numbers
STR(42) → "42"

// Combined with other operators
#define DEBUG(var) char var##_name[] = #var
DEBUG(counter) → char counter_name[] = "counter"
```

### Expected Output Validation
The test suite validates:
- Correct quote placement
- Proper parameter substitution
- Integration with token pasting
- Expression handling
- Number stringification

## Integration Points

### In the Codebase
- **Primary function**: `stringifyParameter()` in `src/macro.c`
- **Integration point**: Parameter substitution loop in `processMacro()`
- **Build system**: Integrated into Makefile with test target

### With Other Features
- **Token pasting**: Compatible with `##` operator (processed in correct order)
- **Recursive macros**: Works with nested macro expansions
- **Conditional compilation**: Functions within `#if` directives
- **File inclusion**: Operates correctly in included files

## Performance Characteristics

- **Time complexity**: O(n) where n is parameter value length
- **Space complexity**: O(1) additional space (fixed buffer)
- **Memory usage**: 512-byte temporary buffer per stringification
- **Processing overhead**: Minimal impact on macro expansion performance

## Examples

### Debugging Macros
```c
#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "Assertion failed: " #expr " at %s:%d\n", \
                    __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)

ASSERT(x > 0) → 
    do { 
        if (!(x > 0)) { 
            fprintf(stderr, "Assertion failed: " "x > 0" " at %s:%d\n", 
                    __FILE__, __LINE__); 
            abort(); 
        } 
    } while(0)
```

### String Table Generation
```c
#define DECLARE_STRING(name) { #name, name }

enum colors { RED, GREEN, BLUE };

struct name_value {
    const char *name;
    int value;
} color_table[] = {
    DECLARE_STRING(RED),    → { "RED", RED }
    DECLARE_STRING(GREEN),  → { "GREEN", GREEN }
    DECLARE_STRING(BLUE),   → { "BLUE", BLUE }
};
```

### Configuration Macros
```c
#define CONFIG_OPTION(name, value) \
    printf("Option " #name " = " #value "\n")

CONFIG_OPTION(MAX_USERS, 100) → 
    printf("Option " "MAX_USERS" " = " "100" "\n")
```

## Future Enhancements

### Planned Improvements
1. **Enhanced escaping**: Support for more escape sequences
2. **Larger buffers**: Configurable or dynamic buffer sizes
3. **Better error reporting**: More detailed error messages
4. **Whitespace normalization**: Optional whitespace collapsing

### Advanced Features
1. **Raw string support**: Alternative stringification modes
2. **Custom escape rules**: Configurable escaping behavior
3. **Multi-line handling**: Better support for complex expressions

The stringification implementation provides robust support for the `#` operator, enabling powerful macro-based code generation and debugging capabilities while maintaining compatibility with standard C preprocessing semantics.
