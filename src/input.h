/**
 * @file input.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>


typedef struct instream {
  struct instream *parent;
  FILE *file;
  char *fname;
  int line;
  int col;
  int pos;
  int last;
  int whitespaces;
  int string;
  char buf[4];
  int eof;
  int error;
} instream_t;


int initsearchdirs();
int addsearchdir(const char *dir);

int newinstream(const char *fname, int flag);
void releaseinstream(instream_t *in);
int readline(instream_t *in, char *buf, int size);
instream_t *getcurrentinstream();


#endif
