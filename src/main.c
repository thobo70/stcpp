
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For getopt()

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
  // Define your supported options here. The colon after each letter indicates that the option requires an argument.
  const char *optString = "D:U:I:";

  if (initsearchdirs() != 0) {
    return 1;
  } 

  while ((opt = getopt(argc, argv, optString)) != -1) {
    switch (opt) {
      case 'D':
        DPRINT("Define macro: %s\n", optarg);
        // Here you would add code to define a macro
        break;
      case 'U':
        DPRINT("Undefine macro: %s\n", optarg);
        // Here you would add code to undefine a macro
        break;
      case 'I':
        DPRINT("Include path: %s\n", optarg);
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
  DPRINT("Input file: %s\n", argv[optind]);
  DPRINT("Output file: %s\n", argv[optind + 1]);

  if (newinstream(argv[optind], 1) != 0) {
    return 1;
  }

  char buf[4096];
  int rtn;
  while ((rtn = readline(NULL, buf, sizeof(buf))) == 0) {
    if (iscmdline(buf)) {
      if (processcmdline(buf, sizeof(buf)) != 0) {
        printf("Error processing command line\n");
        instream_t *in = getcurrentinstream();
        DPRINT("%s(%d, %d): %s\n", in->fname, in->line, in->col, strerror(in->error));
        break;
      }
    } else {
      if (condstate == 0)
        continue;
      if (processBuffer(buf, sizeof(buf)) != 0) {
        printf("Error processing buffer\n");
        break;
      }
      // printf("%1d %03d: %s\n", condstate, in.line, buf);
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
