/**
 * @file input.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief This file contains functions for managing input streams.
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 * This file contains functions for managing input streams, including
 * adding search directories, initializing search directories, checking
 * file paths, and reading characters from input streams.
 * 
 */

#define NDEBUG

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "input.h"

/**
 * @brief Structure representing a search directory.
 */
typedef struct sdir {
  struct sdir *next; /**< Pointer to the next search directory. */
  const char *path;  /**< Path of the search directory. */
} sdir_t;

sdir_t *sdirs = NULL; /**< Head of the linked list of search directories. */
instream_t *currentinstream = NULL; /**< Pointer to the current input stream. */

/**
 * @brief Gets the current input stream.
 * 
 * @return Pointer to the current input stream.
 */
instream_t *getcurrentinstream() {
  return currentinstream;
}

/**
 * @brief Adds a search directory to the list of search directories.
 * 
 * This function allocates memory for a new search directory structure,
 * sets its path, and adds it to the head of the linked list of search directories.
 * 
 * @param path Path of the search directory to add.
 * @return 0 on success, -1 on failure.
 */
int addsearchdir(const char *path) {
  assert(path != NULL);
  sdir_t *dir = malloc(sizeof(sdir_t));
  if (dir == NULL) {
    return -1;
  }
  dir->next = sdirs;
  dir->path = path;
  sdirs = dir;
  DPRINT("Added search directory %s\n", path);
  return 0;
}

/**
 * @brief Initializes the list of search directories from the CPATH environment variable.
 * 
 * This function reads the CPATH environment variable, splits it into individual
 * directory paths, and adds each path to the list of search directories.
 * 
 * @return 0 on success, -1 on failure.
 */
int initsearchdirs() {
  char *cpath = getenv("CPATH");
  if (cpath == NULL) {
    printf("CPATH not set\n");
    return 0;
  }

  //* @todo: check if copy of environment variable is necessary

  char *p = strtok(cpath, ":");
  while (p != NULL) {
    if (addsearchdir(p) != 0) {
      return -1;
    }
    p = strtok(NULL, ":");
  }
  return 0;
}

/**
 * @brief Checks if a file exists in the current directory or any of the search directories.
 * 
 * This function checks if the specified file exists and is readable in the current directory
 * or any of the search directories. If the file is found, it returns the full path to the file.
 * 
 * @param fname Name of the file to check.
 * @param flag If non-zero, check the current directory first.
 * @return Pointer to the full path of the file if found, NULL otherwise.
 */
char *checkpath(const char *fname, int flag) {
  if (fname == NULL) {
    return NULL;
  }
// @todo: if original source file is not in current directory, add path to fname
  if (flag != 0 && access(fname, R_OK) == 0) {
    return strdup(fname);
  }

  sdir_t *dir = sdirs;
  while (dir != NULL) {
    char *pathname = malloc(strlen(dir->path) + strlen(fname) + 2);
    if (pathname == NULL) {
      return NULL;
    }
    strcpy(pathname, dir->path);
    if (pathname[strlen(pathname) - 1] != '/') {
      strcat(pathname, "/");
    }
    strcat(pathname, fname);
    // cppcheck-suppress syntaxError
    // DPRINT("Checking %s\n", pathname);
    if (access(pathname, R_OK) == 0) {
      return pathname;
    }
    free(pathname);
    dir = dir->next;
  }
  return NULL;
}

/**
 * @brief Releases the resources associated with an input stream.
 * 
 * This function closes the file associated with the input stream, frees the memory
 * allocated for the file name, and updates the current input stream to the parent
 * input stream if applicable.
 * 
 * @param in Pointer to the input stream to release.
 */
void releaseinstream(instream_t *in) {
  if (in == NULL) {
    return;
  }
  DPRINT("Releasing current instream '%s'\n", in->fname);
  if (in->file != NULL) {
    fclose(in->file);
  }
  if (in->fname != NULL) {
    free(in->fname);
  }
  if (currentinstream == in) {
    currentinstream = in->parent;
    if (currentinstream != NULL) {
      DPRINT("Current instream is now '%s'\n", currentinstream->fname);
    }
  }
  free(in);
}

/**
 * @brief Creates a new input stream for the specified file.
 * 
 * This function checks if the specified file exists and is readable, opens the file,
 * and initializes a new input stream structure. The new input stream is added to the
 * linked list of input streams.
 * 
 * @param fname Name of the file to open.
 * @param flag If non-zero, check the current directory first.
 * @return 0 on success, -1 on failure.
 */
int newinstream(const char *fname, int flag) {
  char *pathname = checkpath(fname, flag);
  if (pathname == NULL) {
    fprintf(stderr, "File not found: %s\n", fname);
    return -1;
  }
  DPRINT("Opening file %s\n", pathname);
  instream_t *in = malloc(sizeof(instream_t));
  if (in == NULL) {
    return -1;
  }
  in->fname = pathname;
  in->file = fopen(pathname, "r");
  if (in->file == NULL) {
    perror(pathname);
    releaseinstream(in);
    return -1;
  }
  in->line = 1;
  in->col = 0;
  in->pos = 0;
  in->last = 0;
  in->whitespaces = 1;
  in->string = 0;
  in->buf[0] = 0;
  in->buf[1] = 0;
  in->buf[2] = 0;
  in->buf[3] = 0;
  in->eof = 0;
  in->error = 0;
  in->parent = currentinstream;
  currentinstream = in;

  return 0;
}

/**
 * @brief Reads a raw character from the input stream.
 * 
 * This function reads a single character from the input stream without any preprocessing.
 * It updates the line and column numbers and handles end-of-file and error conditions.
 * 
 * @param in Pointer to the input stream.
 * @return The character read, or 0 on end of file, or a negative value on error.
 */
int rawchar(instream_t *in) {
  assert(in != NULL);
  if (in->eof) {
    return 0;
  }
  int c = fgetc(in->file);
  if (c == EOF) {
    in->eof = 1;
    return 0;
  }
  if (c < 0) {
    in->error = errno;
    return c;
  }
  in->col++;
  if (c == '\n') {
    in->line++;
    in->col = 0;
  }
  return c;
}

/**
 * @brief Reads a preprocessed character from the input stream.
 * 
 * This function reads a character from the input stream and applies preprocessing,
 * such as handling string literals, comments, and line continuations.
 * 
 * @param in Pointer to the input stream.
 * @return The character read, or 0 on end of file, or a negative value on error.
 */
int preprocessedchar(instream_t *in) {
  assert(in != NULL);
  if (in->eof) {
    return 0;
  }
  int c = rawchar(in);
  if (c < 0) {
    return c;
  }
  if (in->string) {
    if (c == '\"' && in->last != '\\') {
      in->string = 0;
    }
    return c;
  }
  if (c == '\"' && in->last != '\\') {
    in->string = 1;  // NOLINT
    return c;
  }
  if (c == '/' && in->last != '\\') {
    c = rawchar(in);
    if (c == '/') {
      while (c != '\n' && in->eof == 0) {
        c = rawchar(in);
        if (c < 0) {
          return c;
        }
      }
      return c;
    }
    if (c == '*') {
      c = rawchar(in);
      while (c >= 0) {
        if (c == '*') {
          c = rawchar(in);
          if (c == '/') {
            if (in->whitespaces) {
              return preprocessedchar(in);
            } else {
              in->whitespaces = 1;
              return ' ';
            }
          }
        } else {
          c = rawchar(in);
        }
      }
      return c;
    }
    ungetc(c, in->file);
    c = '/';
  }
  if (c == '\\') {
    c = rawchar(in);
    if (c == '\n') {
      if (in->whitespaces) {
        return preprocessedchar(in);
      }
      in->whitespaces = 1;
      return ' ';
    }
    ungetc(c, in->file);
    return '\\';
  }
  if (c == '\n') {
    in->whitespaces = 1;
    return c;
  }
  if (isspace(c)) {
    while (isspace(c) && in->eof == 0) {
      if (in->whitespaces) {
        c = rawchar(in);
        if (c < 0) {
          return c;
        }
      } else {
        in->whitespaces = 1;
        return ' ';
      }
    }
    ungetc(c, in->file);
    return preprocessedchar(in);
  }
  in->whitespaces = 0;
  return c;
}

/**
 * @brief Reads a character from the input stream, applying preprocessing.
 * 
 * This function reads a character from the input stream, applies preprocessing,
 * and updates the last character read.
 * 
 * @param in Pointer to the input stream.
 * @return The character read, or 0 on end of file, or a negative value on error.
 */
int readchar(instream_t *in) {
  assert(in != NULL);
  if (in->eof) {
    return 0;
  }
  int c = preprocessedchar(in);
  if (c < 0) {
    return c;
  }

  in->last = c;
  return c;
}

/**
 * @brief Reads a line from the input stream into a buffer.
 * 
 * This function reads characters from the input stream into the specified buffer
 * until a newline character or end of file is encountered. It handles preprocessing
 * and updates the current input stream if necessary.
 * 
 * @param in Pointer to the input stream.
 * @param buf Buffer to store the line.
 * @param size Size of the buffer.
 * @return 0 on success, -1 on failure.
 */
int readline(instream_t *in, char *buf, int size) {
  char *end = buf + size - 1;
  int c;

  if (in == NULL) {
    in = currentinstream;
  }
  if (in == NULL) {
    return -1;
  }

  while (buf < end) {
    c = readchar(in);
    if (c < 0) {
      return c;
    }
    if (c == '\n' || c == 0) {
      break;
    }
    *buf++ = c;
  }
  *buf = '\0';
  if (in->eof) {
    releaseinstream(in);
    in = currentinstream;
    if (in == NULL) {
      return -1;
    }
  }
  return 0;
}
