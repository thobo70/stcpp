/**
 * @file input.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "input.h"



typedef struct sdir {
  struct sdir *next;
  const char *path;
} sdir_t;

sdir_t *sdirs = NULL;

instream_t *currentinstream = NULL;


instream_t *getcurrentinstream()
{
  return currentinstream;
}



int addsearchdir(const char *path)
{
  assert(path != NULL);
  sdir_t *dir = malloc(sizeof(sdir_t));
  if (dir == NULL) {
    return -1;
  }
  dir->next = sdirs;
  dir->path = path;
  sdirs = dir;
  return 0;
}



int initsearchdirs()
{
  char *cpath = getenv("CPATH");
  if (cpath == NULL) {
    printf("CPATH not set\n");
    return 0;
  }

  //* @todo: check if copy of environment variable is necessary
  /*
  cpath = strdup(cpath);
  if (cpath == NULL) {
    return -1;
  }
  */

  char *p = strtok(cpath, ":");
  while (p != NULL) {
    if (addsearchdir(p) != 0) {
      return -1;
    }
    p = strtok(NULL, ":");
  }
  return 0;
}



char *checkpath(const char *fname, int flag)
{
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
    DPRINT("Checking %s\n", pathname);
    if (access(pathname, R_OK) == 0) {
      return pathname;
    }
    free(pathname);
    dir = dir->next;
  }
  return NULL;
}



void releaseinstream(instream_t *in)
{
  if (in == NULL) {
    return;
  }
  if (in->file != NULL) {
    fclose(in->file);
  }
  if (in->fname != NULL) {
    free(in->fname);
  }
  if (currentinstream == in) {
    currentinstream = in->parent;
  }
  free(in);
}



int newinstream(const char *fname, int flag)
{
  char *pathname = checkpath(fname, flag);
  if (pathname == NULL) {
    fprintf(stderr, "File not found: %s\n", fname);
    return -1;
  }
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



int rawchar(instream_t *in)
{
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



int preprocessedchar(instream_t *in)
{
  assert(in != NULL);
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
      while (c != '\n') {
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
    while (isspace(c)) {
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



int readchar(instream_t *in)
{
  assert(in != NULL);
  int c = preprocessedchar(in);
  if (c < 0) {
    return c;
  }

  in->last = c;
  return c;
}



int readline(instream_t *in, char *buf, int size)
{
  char *start = buf, *end = buf + size - 1;
  int c;

  if (in == NULL) {
    in = currentinstream;
  }
  if (in == NULL) {
    return -1;
  }
  if (in->eof) {
    if (in->parent == NULL) {
      return 1;
    }
    currentinstream = in->parent;
  }

  while (buf < end) {
    c = readchar(in);
    if (c < 0) {
      return c;
    }
    if (c == '\n') {
      *buf = '\0';
      if (buf > start) {
        return 0;
      }
      continue;
    }
    *buf++ = c;
    if (c == 0) {
      return 1;
    }
  }
  *buf = '\0';
  return 0;
}
