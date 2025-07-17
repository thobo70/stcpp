# Token Pasting (##) Implementation

## Overview

This document describes the implementation of the token pasting operator (`##`) in the stcpp C preprocessor. The `##` operator is used to concatenate tokens during macro expansion, allowing the creation of new identifiers and tokens at preprocessing time.

## Implementation Details

### Core Function: `removeDoubleHash()`

The token pasting functionality is implemented in the `removeDoubleHash()` function in `src/macro.c`. Despite its legacy name, this function now performs proper token concatenation rather than simply removing the `##` operators.

**Location in processing**: Token pasting occurs after parameter substitution in function-like macros, which is the correct point in the C preprocessing pipeline.

### Algorithm

1. **Search for ## operators**: Scan through the macro expansion result looking for `##` sequences
2. **Identify tokens**: For each `##` found:
   - Find the left token by scanning backwards from `##`, skipping whitespace
   - Find the right token by scanning forwards from `##`, skipping whitespace
   - Token boundaries are determined by alphanumeric characters and underscores
3. **Concatenate tokens**: Replace the entire sequence `left_token ## right_token` with `left_tokenright_token`
4. **Update buffer**: Adjust the buffer content and continue processing

### Supported Use Cases

#### Basic Token Concatenation
```c
#define CONCAT(a, b) a ## b
int CONCAT(hello, world) = 42;  // Results in: int helloworld = 42;
```

#### Prefix/Suffix Generation
```c
#define MAKE_VAR(num) var ## num
int MAKE_VAR(1) = 100;          // Results in: int var1 = 100;
```

#### Multiple Concatenation
```c
#define TRIPLE(a, b, c) a ## b ## c
int TRIPLE(x, y, z) = 999;      // Results in: int xyz = 999;
```

#### Number Concatenation
```c
#define MAKE_VAR(num) var ## num
int MAKE_VAR(42) = 200;         // Results in: int var42 = 200;
```

## Technical Considerations

### Processing Order
1. Macro replacement text is substituted into the buffer
2. Function-like macro parameters are replaced with their arguments
3. Token pasting (`##`) is performed on the result
4. Recursive macro expansion continues if needed

### Buffer Management
- The implementation carefully manages buffer boundaries to prevent overflows
- Memory is moved efficiently using `memmove()` to handle overlapping regions
- Token length is limited to 256 characters for safety

### Error Handling
- If token concatenation would exceed buffer limits, the `##` is simply removed
- Invalid pasting contexts (no valid tokens on either side) result in `##` removal
- The implementation gracefully handles edge cases without crashing

## Limitations

### Current Limitations
1. **Complex token types**: Only handles simple alphanumeric tokens and underscores
2. **Whitespace handling**: Basic whitespace skipping around `##`
3. **Error reporting**: Limited error messages for invalid pasting operations

### Not Implemented
- **Stringizing operator** (`#`): Not yet implemented
- **Empty token handling**: Limited support for edge cases with empty tokens
- **Complex token boundaries**: Only basic identifier characters supported

## Testing

### Test Coverage
The token pasting implementation is tested through:
- `test/test_token_pasting.c`: Comprehensive test cases
- Integration with the test suite via `make test-all`
- Individual testing via `make test-token-pasting`

### Test Cases Include
- Basic two-token concatenation
- Identifier-number concatenation  
- Multiple token concatenation (a##b##c)
- Integration with function-like macros

## Integration Points

### In the Codebase
- **Function**: `removeDoubleHash()` in `src/macro.c`
- **Called from**: `processMacro()` after parameter substitution
- **Build system**: Integrated into Makefile with test target

### With Other Features
- **Macro expansion**: Works seamlessly with recursive macro expansion
- **Parameter substitution**: Operates after parameter replacement
- **Conditional compilation**: Compatible with `#if` directive processing

## Performance Characteristics

- **Time complexity**: O(n) where n is the length of the macro expansion
- **Space complexity**: O(1) additional space (operates in-place)
- **Memory usage**: Temporary buffer of 256 bytes for token assembly

## Future Enhancements

### Planned Improvements
1. **Enhanced token recognition**: Support for more token types
2. **Better error reporting**: More descriptive error messages
3. **Stringizing operator**: Implementation of the `#` operator
4. **Edge case handling**: Better support for complex pasting scenarios

### Compatibility
The implementation follows C99/C11 preprocessing semantics for token pasting, ensuring compatibility with standard C code that uses the `##` operator.

## Examples

### Working Examples
```c
// Basic concatenation
#define JOIN(a,b) a##b
JOIN(hello,world) → helloworld

// Variable generation  
#define VAR(n) variable##n
VAR(42) → variable42

// Multi-token
#define ABC(a,b,c) a##b##c  
ABC(x,y,z) → xyz
```

### Integration with Recursive Macros
```c
#define INNER(x) var##x
#define OUTER(n) INNER(n)
OUTER(123) → var123
```

The token pasting implementation successfully handles these cases and integrates well with the existing macro processing pipeline in stcpp.
