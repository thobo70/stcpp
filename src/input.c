
#include <ctype.h>
#include <errno.h>
#include "input.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>


instream_t *currentinstream = NULL;


instream_t *getcurrentinstream()
{
  return currentinstream;
}



char *checkpath(const char *fname, int flag)
{
  if (fname == NULL) {
    return NULL;
  }
  if (flag != 0 && access(fname, R_OK) == 0) {
    return strdup(fname);
  }
  char *cpath = getenv("CPATH");
  if (cpath == NULL) {
    printf("CPATH not set\n");
    return NULL;
  }
  cpath = strdup(cpath);

  char *p = strtok(cpath, ":");
  while(p != NULL) {
    char *pathname = malloc(strlen(p) + strlen(fname) + 2);
    if (pathname == NULL) {
      free(cpath);
      return NULL;
    }
    strcpy(pathname, p);
    if (pathname[strlen(pathname) - 1] != '/') {
      strcat(pathname, "/");
    }
    strcat(pathname, fname);
    printf("Checking %s\n", pathname);
    if (access(pathname, R_OK) == 0) {
      free(cpath);
      return pathname;
    }
    free(pathname);
    p = strtok(NULL, ":");
  }
  free(cpath);
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
    printf("File not found: %s\n", fname);
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
    in->string = 1;
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



/*
int main()
{
  char *fname = "test.h";
  instream_t in;
  in.file = fopen(fname, "r");
  in.fname = fname;
  in.line = 1;
  in.col = 0;
  in.pos = 0;
  in.last = 0;
  in.whitespaces = 1;
  in.buf[0] = 0;
  in.buf[1] = 0;
  in.buf[2] = 0;
  in.buf[3] = 0;
  in.eof = 0;
  in.error = 0;

  if (in.file == NULL) {
    perror(in.fname);
    return 1;
  }

  printf("Reading file %s\n", in.fname);
  char buf[4096];
  while (readline(&in, buf, sizeof(buf)) == 0) {
    printf("%03d: %s\n", in.line, buf);
  }
  printf("%s: %s\n", in.fname, strerror(in.error));

  return 0;
}
*/