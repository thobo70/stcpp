# Stdin/Stdout Support Implementation Documentation

## Overview

This document describes the implementation of stdin/stdout support in stcpp, enabling the preprocessor to work seamlessly in Unix-style pipelines. This feature allows reading from standard input and writing to standard output using the special filename `-`.

## Implementation Details

### Core Functionality

The stdin/stdout support is implemented in two main components:

1. **Input Stream Handling**: Modified `src/input.c` to handle stdin as a special file stream
2. **Command Line Processing**: Enhanced `src/main.c` to detect `-` filenames and route them appropriately

### Technical Implementation

#### Input Stream Modifications (`src/input.c`)

**Enhanced `newinstream()` Function:**
```c
int newinstream(const char *fname, int flag) {
  char *pathname;
  FILE *file;
  
  // Handle stdin special case
  if (strcmp(fname, "<stdin>") == 0) {
    pathname = strdup("<stdin>");
    file = stdin;
  } else {
    // Regular file handling
    pathname = checkpath(fname, flag);
    file = fopen(pathname, "r");
  }
  // ... rest of initialization
}
```

**Enhanced `releaseinstream()` Function:**
```c
void releaseinstream(instream_t *in) {
  // ... existing code ...
  if (in->file != NULL && in->file != stdin) {
    fclose(in->file);  // Don't close stdin
  }
  // ... rest of cleanup
}
```

#### Command Line Processing (`src/main.c`)

**Stdin Detection Logic:**
```c
if (strcmp(infname, "-") == 0) {
  // Use stdin
  if (newinstream(strdup("<stdin>"), 0) != 0) {
    return 1;
  }
} else {
  // Use regular file
  if (newinstream(strdup(infname), 1) != 0) {
    return 1;
  }
}
```

**Stdout Handling:**
The stdout functionality was already present in the original code:
```c
FILE *outfile = stdout;  // Default to stdout
if (strcmp(outfname, "-") != 0) {
  outfile = fopen(outfname, "w");  // Only open file if not "-"
}
```

### Features and Capabilities

#### Pipeline Support
- **Full Pipeline**: `echo 'code' | stcpp - -`
- **Input from Stdin**: `cat input.c | stcpp - output.c`
- **Output to Stdout**: `stcpp input.c -`
- **Complex Workflows**: `cat input.c | stcpp -DDEBUG=1 - - | gcc -x c - -o program`

#### Compatibility
- **Option Compatibility**: All command-line options (-D, -U, -I, -h) work with stdin/stdout
- **Backward Compatibility**: Existing file-based workflows unchanged
- **Error Handling**: Proper error messages and resource cleanup

### Resource Management

#### Memory Management
- **Stdin Streams**: Proper allocation and cleanup without closing stdin
- **String Handling**: Safe duplication of `<stdin>` filename identifier
- **Error Handling**: Cleanup of resources on failure paths

#### File Descriptor Management
- **Stdin Protection**: Prevents accidental closing of stdin file descriptor
- **Regular Files**: Normal file closing behavior preserved
- **Stream State**: Proper tracking of stream types and cleanup requirements

### Testing and Validation

#### Test Coverage
- **Basic Pipeline**: Input from stdin, output to stdout
- **Mixed Usage**: File input with stdout output, and vice versa
- **Macro Processing**: Ensures all preprocessing features work through stdin/stdout
- **Error Conditions**: Proper handling of invalid input or processing errors

#### Integration Testing
- **Command Line Options**: Verified compatibility with all existing options
- **File Operations**: Ensured no regression in regular file processing
- **Resource Cleanup**: Verified proper cleanup in all scenarios

## Usage Examples

### Basic Pipeline Usage
```bash
# Simple macro expansion
echo '#define PI 3.14159' | stcpp - -

# Complex preprocessing
echo -e '#define MAX(a,b) ((a)>(b)?(a):(b))\nint result = MAX(10, 20);' | stcpp - -
```

### Integration with Build Systems
```bash
# Preprocess and compile in one pipeline
cat source.c | stcpp -DDEBUG=1 -Iinclude - - | gcc -x c - -o program

# Preprocess with multiple includes
find . -name "*.c" -exec cat {} \; | stcpp -Isrc -Iinclude - preprocessed.c
```

### Development Workflows
```bash
# Quick macro testing
echo '#define TEST __LINE__' | stcpp - -

# Conditional compilation testing
echo -e '#ifdef DEBUG\nprintf("debug");\n#endif' | stcpp -DDEBUG=1 - -
```

## Error Handling

### Input Validation
- **Stream Availability**: Proper handling when stdin is not available
- **File Permissions**: Standard file permission error handling maintained
- **Memory Allocation**: Graceful handling of memory allocation failures

### Resource Cleanup
- **Stdin Preservation**: Ensures stdin remains open for subsequent operations
- **Memory Deallocation**: Proper cleanup of allocated pathname strings
- **Error Recovery**: Clean shutdown on processing errors

## Performance Considerations

### Memory Usage
- **Stream Overhead**: Minimal additional memory for stdin stream handling
- **Buffer Management**: Standard buffer sizes maintained for stdin processing
- **Resource Efficiency**: No performance penalty for regular file operations

### Processing Speed
- **Stream Processing**: Stdin processing speed comparable to file processing
- **Pipeline Efficiency**: Optimized for continuous data flow in pipelines
- **Memory Copying**: Minimal data copying for stdin operations

## Standards Compliance

### Unix Philosophy
- **Pipeline Friendly**: Follows Unix tradition of chainable command-line tools
- **Standard Streams**: Proper use of stdin, stdout, and stderr
- **Exit Codes**: Standard exit code behavior maintained

### POSIX Compatibility
- **File Descriptor Handling**: POSIX-compliant file descriptor management
- **Signal Handling**: Graceful handling of pipeline interruptions
- **Process Behavior**: Standard process termination and cleanup

## Future Enhancements

### Potential Improvements
- **Stderr Redirection**: Support for `-` as stderr filename
- **Binary Mode**: Enhanced handling for binary input streams
- **Buffering Control**: Options for controlling input/output buffering
- **Stream Multiplexing**: Support for multiple input streams

### Integration Opportunities
- **IDE Integration**: Enhanced support for IDE preprocessing workflows
- **Build System Integration**: Tighter integration with modern build systems
- **Development Tools**: Enhanced debugging and profiling support

The stdin/stdout support implementation provides a robust foundation for pipeline-based preprocessing workflows while maintaining full compatibility with existing file-based operations.
