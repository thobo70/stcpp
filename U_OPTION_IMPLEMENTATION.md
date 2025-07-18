# Command Line Macro Undefinition (-U) Implementation

## Overview

The `-U` command line option allows users to undefine preprocessor macros from the command line, providing the ability to remove macro definitions that were previously defined with `-D` or in source code.

## Implementation Details

### Integration with Existing deleteMacro() Function

**Location**: `src/main.c`

The `-U` option implementation leverages the existing `deleteMacro()` function from `src/macro.c`:

```c
case 'U':
    DPRINT("Undefine macro: %s\n", optarg);
    if (deleteMacro(optarg) != 0) {
        fprintf(stderr, "Warning: Macro '%s' not defined, cannot undefine\n", optarg);
        // Note: This is typically a warning, not an error in most C preprocessors
    }
    break;
```

### Existing deleteMacro() Function

**Location**: `src/macro.c`

The `deleteMacro()` function handles:
- Searching the macro list for the specified macro name
- Removing the macro from the linked list
- Properly freeing all associated memory (name, replacement text, parameters)
- Returning 0 on success, -1 if macro not found

## Supported Operations

### 1. Undefine Previously Defined Macro
```bash
./stcpp -DDEBUG=1 -UDEBUG input.c output.c
```
- Defines `DEBUG` as `1`, then immediately undefines it
- Result: `DEBUG` is not defined during preprocessing

### 2. Undefine Multiple Macros
```bash
./stcpp -DDEBUG -DVERSION=2 -UDEBUG -UVERSION input.c output.c
```
- Supports multiple `-U` options in a single command
- Each macro is undefined individually

### 3. Mixed Define/Undefine Operations
```bash
./stcpp -DDEBUG=5 -UDEBUG -DDEBUG=10 input.c output.c
```
- Define, undefine, then redefine sequence
- Final result: `DEBUG` has value `10`

### 4. Warning for Non-Existent Macros
```bash
./stcpp -UNONEXISTENT input.c output.c
```
- Shows warning: "Macro 'NONEXISTENT' not defined, cannot undefine"
- Continues processing (warning, not error)

## Order of Operations

Command line options are processed left-to-right:

1. **-DDEBUG=1 -UDEBUG**: DEBUG is defined then undefined (final state: undefined)
2. **-UDEBUG -DDEBUG=1**: Attempt to undefine (warning), then DEBUG is defined (final state: defined)
3. **-DDEBUG=5 -UDEBUG -DDEBUG=10**: Define, undefine, redefine (final state: DEBUG=10)

## Testing

### Test Cases

The implementation includes comprehensive tests covering all scenarios:

**Test Files:**
- `test/test_u_option.c` - Conditional compilation test file
- `test/test_u_option.sh` - Comprehensive test script

### Test Scenarios

1. **Normal Definition Test**:
   ```bash
   ./stcpp -DDEBUG=1 -DVERSION=42 test/test_u_option.c output.c
   ```
   Result: Both macros defined and used

2. **Single Macro Undefinition**:
   ```bash
   ./stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG test/test_u_option.c output.c
   ```
   Result: DEBUG undefined, VERSION still defined

3. **Multiple Macro Undefinition**:
   ```bash
   ./stcpp -DDEBUG=1 -DVERSION=42 -UDEBUG -UVERSION test/test_u_option.c output.c
   ```
   Result: Both macros undefined

4. **Non-Existent Macro Warning**:
   ```bash
   ./stcpp -UNONEXISTENT test/test_u_option.c output.c
   ```
   Result: Warning message, processing continues

5. **Define-Undefine-Redefine Sequence**:
   ```bash
   ./stcpp -DDEBUG=5 -UDEBUG -DDEBUG=10 test/test_u_option.c output.c
   ```
   Result: Final DEBUG value is 10

### Integration Testing

The `-U` option works correctly with:
- Conditional compilation (`#ifdef`, `#ifndef`, `#if`)
- Mixed `-D` and `-U` operations
- Multiple macro operations in single command
- Built-in macros and existing functionality
- Pipeline operations (stdin/stdout)

## Error Handling

The implementation provides appropriate error handling:
- **Warning for undefined macros**: Shows warning but continues processing
- **Proper memory cleanup**: Uses existing `deleteMacro()` function's memory management
- **Return code handling**: Distinguishes between success (0) and not found (-1)

## Compatibility

This implementation maintains compatibility with:
- Standard C preprocessor `-U` option behavior
- Existing stcpp command line options (`-D`, `-I`, `-h`)
- All existing macro functionality
- Current test infrastructure and build system

## Memory Management

The `-U` option leverages existing memory management:
- Uses `deleteMacro()` function for proper cleanup
- Frees macro name, replacement text, and parameter lists
- No memory leaks introduced
- Maintains integrity of macro list structure

## Standard Compliance

The implementation follows standard C preprocessor behavior:
- Warning (not error) for undefined macros
- Left-to-right processing of command line options
- Proper integration with conditional compilation
- Compatible syntax and semantics

## Performance Considerations

- **Efficient lookup**: Uses existing macro list search mechanism
- **Minimal overhead**: Direct call to `deleteMacro()` function
- **No impact on preprocessing**: Only affects command line parsing phase
- **Memory efficiency**: Immediately frees undefined macro memory

## Future Enhancements

Potential improvements for future development:
1. **Quiet mode**: Option to suppress warnings for undefined macros
2. **Wildcard undefinition**: Support patterns like `-U"DEBUG_*"`
3. **Batch operations**: Read macro names from file
4. **Validation mode**: Check if macros exist before processing

## Conclusion

The `-U` option implementation provides complete standard C preprocessor compatibility for macro undefinition while maintaining the simplicity and efficiency of the stcpp preprocessor. The implementation leverages existing infrastructure and provides robust error handling and comprehensive testing.
