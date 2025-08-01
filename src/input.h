/**
 * @file input.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Header file for input stream management functionality.
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This header file contains structures and function declarations for managing
 * input streams, including file inclusion, search path management, and 
 * preprocessed character reading with comment and line continuation handling.
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

/**
 * @struct instream
 * @brief Structure representing an input stream.
 * 
 * This structure maintains the state of an input stream, including file handle,
 * position tracking, and preprocessing state. Input streams can be nested
 * to support file inclusion through the parent pointer.
 */
typedef struct instream {
  struct instream *parent; /**< Parent input stream for nested includes. */
  FILE *file;              /**< File handle for the input stream. */
  char *fname;             /**< Filename of the input stream. */
  int line;                /**< Current line number (1-based). */
  int col;                 /**< Current column number (0-based). */
  int pos;                 /**< Current position in the file. */
  int last;                /**< Last character read. */
  int whitespaces;         /**< Flag indicating if in whitespace sequence. */
  int string;              /**< Flag indicating if inside string literal. */
  char buf[4];             /**< Character buffer for lookahead. */
  int eof;                 /**< End-of-file flag. */
  int error;               /**< Error code from last operation. */
} instream_t;

/**
 * @brief Initializes search directories from CPATH environment variable.
 * 
 * Reads the CPATH environment variable and splits it into individual
 * directory paths, adding each to the search directory list.
 * 
 * @return 0 on success, -1 on failure.
 */
int initsearchdirs();

/**
 * @brief Adds a directory to the include search path.
 * 
 * Adds the specified directory to the head of the search directory list.
 * Used to implement the -I command line option.
 * 
 * @param path Path of the directory to add to search path.
 * @return 0 on success, -1 on failure (memory allocation error).
 */
int addsearchdir(const char *dir);

/**
 * @brief Creates a new input stream for the specified file.
 * 
 * Searches for the file in the current directory and search paths,
 * opens it, and creates a new input stream. The new stream becomes
 * the current input stream, with the previous stream as its parent.
 * 
 * @param fname Name of the file to open.
 * @param flag If non-zero, check current directory first; if zero, only check search paths.
 * @return 0 on success, -1 on failure (file not found, cannot open, memory allocation error).
 */
int newinstream(const char *fname, int flag);

/**
 * @brief Releases resources associated with an input stream.
 * 
 * Closes the file, frees allocated memory, and restores the parent
 * input stream as the current stream. Used when reaching end-of-file
 * or encountering errors.
 * 
 * @param in Pointer to the input stream to release.
 */
void releaseinstream(instream_t *in);

/**
 * @brief Reads a line from the input stream into a buffer.
 * 
 * Reads characters from the current input stream until a newline
 * or end-of-file is encountered. Handles preprocessed input with
 * comment removal and line continuation processing.
 * 
 * @param in Input stream to read from (NULL to use current stream).
 * @param buf Buffer to store the line.
 * @param size Size of the buffer.
 * @return 0 on success, negative value on error.
 */
int readline(instream_t *in, char *buf, int size);

/**
 * @brief Gets the current input stream.
 * 
 * Returns a pointer to the currently active input stream.
 * 
 * @return Pointer to current input stream, or NULL if none active.
 */
instream_t *getcurrentinstream();

/**
 * @brief Sets the line number and filename for the current input stream.
 * 
 * Implements the #line directive functionality by updating the line number
 * and optionally the filename of the current input stream.
 * 
 * @param linenum New line number to set (1-based).
 * @param filename New filename to set, or NULL to keep current filename.
 */
void setlinenumber(int linenum, const char *filename);

/**
 * @brief Generates a #line directive for the next compiler stage.
 * 
 * Outputs a #line directive to the specified file to maintain correct
 * line number tracking through the preprocessing chain.
 * 
 * @param outfile Output file to write the directive to.
 * @param linenum Line number to set (1-based).
 * @param filename Filename to set, or NULL to omit filename.
 */
void generate_line_directive(FILE *outfile, int linenum, const char *filename);


#endif
