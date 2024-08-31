/**
 * @file main.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#define NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For getopt()

#include "debug.h"
#include "input.h"
#include "macro.h"
#include "cmdline.h"

/*
write a function that takes the command line arguments and processes them
command line options: cpp [-Dname[=value]] [-Uname] [-Ipath] infile outfile
if infile is specified with '-', stdin is used
if outfile is specified with '-', stdout is used
-Dname: Define a macro named name with a value of 1. You can also specify a value with -Dname=value.
-Uname: Undefine the macro name.
-Ipath: Add the directory path to the list of directories to be searched for header files.
*/

int main(int argc, char *argv[])
{
  int opt;
  FILE *outfile = stdout;
  char *outfname = NULL, *infname = NULL;
  // Define your supported options here. The colon after each letter indicates that the option requires an argument.
  const char *optString = "D:U:I:";

  if (initsearchdirs() != 0) {
    return 1;
  }

  char *oarg = NULL;
  while ((opt = getopt(argc, argv, optString)) != -1) {
    switch (opt) {
      case 'D':
        oarg = optarg;
        // cppcheck-suppress syntaxError
        DPRINT("Define macro: %s\n", oarg);
        addMacro(oarg);   // @todo: addMacro() is not enough, you need to parse the optarg for name and value
        // Here you would add code to define a macro
        break;
      case 'U':
        DPRINT("Undefine macro: %s\n", optarg);
        // Here you would add code to undefine a macro
        break;
      case 'I':
        addsearchdir(optarg);
        break;
     default:
        // Handle unknown options and missing option arguments
        fprintf(stderr, "Unknown option or missing option argument: %c\n", opt);
        break;
    }
  }

  if (optind != argc - 2) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "cpp [-Dname[=value]] [-Uname] [-Ipath] infile outfile\n");
    return 1;
  }
  infname = argv[optind];
  outfname = argv[optind + 1];
  DPRINT("Input file: %s\n", infname);
  DPRINT("Output file: %s\n", outfname);
  if (strcmp(outfname, "-") != 0) {
    outfile = fopen(outfname, "w");
    if (outfile == NULL) {
      perror(outfname);
      return 1;
    }
  }


  if (newinstream(strdup(infname), 1) != 0) {
    return 1;
  }

  char buf[4096];
  int rtn;
  while ((rtn = readline(NULL, buf, sizeof(buf))) == 0) {
    instream_t *in = getcurrentinstream();
    if (in->eof) {
      break;
    }
    if (iscmdline(buf)) {
      if (processcmdline(buf, sizeof(buf)) != 0) {
        printf("Error processing command line\n");
        DPRINT("%s(%d, %d): %s\n", in->fname, in->line, in->col, strerror(in->error));
        break;
      }
    } else {
      DPRINT("> %1d %03d: %s\n", condstate, in->line, buf);
      if (condstate == 0)
        continue;
      if (processBuffer(buf, sizeof(buf), 0) != 0) {
        printf("Error processing buffer\n");
        break;
      }
      strcat(buf, "\n");
      fputs(buf, outfile);
    }
  }
  if (rtn < 0) {
    printf("Error reading file\n");
  }
  // printf("%s: %s\n", in.fname, strerror(in.error));

#ifndef NDEBUG
  printMacroList();
#endif

  return 0;
}
