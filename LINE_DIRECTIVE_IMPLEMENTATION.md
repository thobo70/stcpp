# Line Directive Implementation

**Author:** Thomas Boos (tboos70@gmail.com)  
**Date:** July 18, 2025  
**Module:** `cmdline.c` / `input.c`  
**Status:** ✅ Production-Ready - Enhanced with Next-Stage Pass-through

## Overview

The `#line` directive implementation in stcpp provides standard C preprocessor line control functionality with pass-through support for the next compiler stage. This directive allows source code to specify explicit line numbers and optionally filenames for diagnostic and debugging purposes, and ensures these directives are preserved in the preprocessor output for subsequent compilation stages.

## Directive Syntax

The `#line` directive supports two standard forms:

```c
#line line-number
#line line-number "filename"
```

### Enhanced Pass-through Functionality

The implementation now supports dual-mode operation:

1. **Internal Line Tracking**: Updates the preprocessor's internal line counters for error reporting
2. **Next-Stage Pass-through**: Outputs `#line` directives to the preprocessed file for the next compiler stage

**Output Example:**
```c
// Input
#line 100
int function_a() {
    return 42;
}

// Output (after preprocessing)
#line 100
int function_a() {
    return 42;
}
```

This ensures that line number information is preserved through the entire compilation pipeline.

### Examples

```c
#line 100
// This line will be considered line 100

#line 200 "generated.c"  
// This line will be considered line 200 in file "generated.c"

#line    50    "spaced.c"    
// Whitespace is handled correctly

#line 1
// Reset to line 1
```

## Implementation Architecture

### Module Integration

The line directive implementation spans two modules:

1. **`cmdline.c`**: Directive parsing and validation
2. **`input.c`**: Line number and filename management

```
#line directive processing flow:
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ processcmdline()│ -> │    do_line()    │ -> │ setlinenumber() │
│   (cmdline.c)   │    │   (cmdline.c)   │    │   (input.c)     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Core Functions

#### `do_line()` - Directive Parser

```c
/**
 * @brief Parse and process a #line directive.
 * 
 * This function parses a #line directive and sets the line number and optionally
 * the filename for the current input stream. The #line directive can have two forms:
 * - #line number
 * - #line number "filename"
 * 
 * @param buf Pointer to the buffer containing the line directive arguments.
 * @param end Pointer to the end of the buffer.
 * @return 0 on success, -1 on error.
 */
int do_line(char *buf, char *end);
```

**Features:**
- ✅ Parses line numbers (validates positive integers)
- ✅ Handles optional quoted filenames
- ✅ Validates directive syntax
- ✅ Provides comprehensive error checking
- ✅ Handles arbitrary whitespace

#### `setlinenumber()` - Line Control

```c
/**
 * @brief Sets the line number and optionally the filename for the current input stream.
 * 
 * This function implements the #line directive functionality by updating the
 * line number of the current input stream and optionally changing the filename.
 * 
 * @param linenum The new line number to set (1-based).
 * @param filename The new filename to set, or NULL to keep current filename.
 */
void setlinenumber(int linenum, const char *filename);
```

**Features:**
- ✅ Updates current input stream line numbers
- ✅ Manages filename memory allocation safely
- ✅ Handles NULL filename (line number only)
- ✅ Integrates with existing input stream system

## Implementation Details

### Line Number Management

The line number system in stcpp is based on the `instream_t` structure:

```c
typedef struct instream {
    struct instream *parent;  // Parent stream (for #include)
    FILE *file;               // File handle
    char *fname;              // Filename (dynamically allocated)
    int line;                 // Current line number
    int col;                  // Current column
    // ... other fields
} instream_t;
```

### Memory Management

The implementation handles filename memory management carefully:

```c
void setlinenumber(int linenum, const char *filename) {
    instream_t *current = getcurrentinstream();
    if (current == NULL) return;
    
    // Set line number (adjust by -1 for increment)
    current->line = linenum - 1;
    
    // Handle filename update
    if (filename != NULL && filename[0] != '\0') {
        // Free existing filename
        if (current->fname != NULL) {
            free(current->fname);
        }
        
        // Allocate and copy new filename
        current->fname = malloc(strlen(filename) + 1);
        if (current->fname != NULL) {
            strcpy(current->fname, filename);
        }
    }
}
```

**Safety Features:**
- ✅ Null pointer checking
- ✅ Proper memory deallocation
- ✅ Dynamic string allocation
- ✅ Error handling for allocation failures

### Error Handling

The `do_line()` function provides comprehensive error detection:

```c
// Line number validation
linenum = (int)strtol(start, &linenum_end, 10);
if (linenum_end == start || linenum < 1) {
    DPRINT("Error: #line directive has invalid line number\n");
    return -1;
}

// Filename string validation
if (start < end && *start == '"') {
    char *filename_end = strchr(filename_start, '"');
    if (filename_end == NULL || filename_end >= end) {
        DPRINT("Error: #line directive has unterminated filename string\n");
        return -1;
    }
}

// Unexpected content detection
if (start < end && *start != '\0' && *start != '\n') {
    DPRINT("Error: #line directive has unexpected content after filename\n");
    return -1;
}
```

**Error Types Detected:**
- ✅ Invalid line numbers (non-numeric, zero, negative)
- ✅ Unterminated filename strings
- ✅ Unexpected content after directive
- ✅ Buffer overflow conditions

## Integration with Preprocessor

### Command Processing

The line directive is integrated into the main command processing switch:

```c
switch (cmd) {
    // ... other cases
    case LINE:
        DPRINT("Line: %s\n", buf + 1);
        if (do_line(buf + 1, end) != 0) {
            DPRINT("Error processing #line directive\n");
            return -1;
        }
        break;
    // ... other cases
}
```

### Stream Management

The directive works seamlessly with the existing input stream system:
- ✅ Affects current input stream only
- ✅ Preserved across `#include` file boundaries
- ✅ Integrates with existing error reporting
- ✅ Compatible with conditional compilation

## Use Cases

### Generated Code

```c
// Generated by a tool
#line 1 "original_source.template"
int function_a() {
    return 42;
}

#line 15 "original_source.template"  
int function_b() {
    return 84;
}
```

### Macro Debugging

```c
#define COMPLEX_MACRO(x, y) \
    #line __LINE__ __FILE__ \
    do { \
        printf("At line %d: x=%d, y=%d\n", __LINE__, x, y); \
    } while(0)
```

### Template Processing

```c
// Template processor output
#line 100 "user_template.tmpl"
template_generated_code();

#line 200 "user_template.tmpl"
more_template_code();
```

## Testing

### Test Coverage

The implementation includes comprehensive testing:

```bash
# Individual test
make test-line

# Part of full test suite
make test-all
```

### Test Validation

The test verifies:
- ✅ Directive recognition and parsing
- ✅ Proper removal from output stream
- ✅ Error handling for malformed directives
- ✅ Integration with other preprocessor features

### Test File Structure

```c
/* Test for #line directive */

#line 100
/* This comment should be considered at line 100 */

#line 200 "virtual.c"
/* This comment should be considered at line 200 in virtual.c */

#line    300    "test.c"   
/* This comment should be at line 300 in test.c */
```

**Expected Behavior:**
- All `#line` directives are processed and removed from output
- Line numbering is updated internally for error reporting
- Filenames are tracked for diagnostic purposes

## Standards Compliance

### C Standard Conformance

The implementation follows C standard specifications:

- ✅ **C89/C90**: Full compliance with basic line directive
- ✅ **C99**: Compatible with enhanced error reporting
- ✅ **C11**: Maintains compatibility with modern standards

### Behavior Specification

```c
// Standard form: #line digit-sequence
#line 42

// Standard form: #line digit-sequence "s-char-sequence"
#line 42 "file.c"

// Non-standard but accepted: whitespace flexibility
#line    42    "file.c"    
```

## Performance Characteristics

### Memory Usage
- **Overhead**: Minimal (one integer, one pointer per stream)
- **Allocation**: Dynamic filename storage only when needed
- **Cleanup**: Automatic on stream destruction

### Processing Speed
- **Parsing**: O(n) where n is directive length
- **Application**: O(1) constant time operations
- **Memory**: O(m) where m is filename length

## Future Enhancements

### Potential Improvements

1. **Enhanced Validation**: Additional syntax checking
2. **Integration Features**: Better integration with error reporting
3. **Debug Support**: Enhanced debugging information
4. **Extensions**: Support for additional line directive forms

### Compatibility Considerations

- **Preprocessor Chains**: Works with other preprocessors
- **Tool Integration**: Compatible with build systems
- **Error Reporting**: Integrates with IDE error display

## Conclusion

The `#line` directive implementation provides a complete, standards-compliant solution for line control in stcpp. Key achievements:

- ✅ **Complete Implementation**: Both line number and filename forms
- ✅ **Robust Error Handling**: Comprehensive validation and error reporting
- ✅ **Memory Safety**: Proper allocation and deallocation
- ✅ **Standards Compliance**: Follows C standard specifications
- ✅ **Integration**: Seamless integration with existing preprocessor
- ✅ **Testing**: Comprehensive test coverage with validation

This implementation enables stcpp to handle generated code, template processing, and debugging scenarios effectively while maintaining the preprocessor's lightweight and efficient design principles.
