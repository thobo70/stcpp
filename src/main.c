
#include "input.h"
#include "macro.h"
#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For getopt()

/*
write a function that takes the command line arguments and processes them
command line options: cpp [-Dname[=value]] [-Uname] [-Ipath] infile outfile
if infile is specified with '-', stdin is used
if outfile is specified with '-', stdout is used
-Dname: Define a macro named name with a value of 1. You can also specify a value with -Dname=value.
-Uname: Undefine the macro name.
-Ipath: Add the directory path to the list of directories to be searched for header files.
-include file: Include the file before processing the main source file.
-nostdinc: Do not search the standard system directories for header files. Only the directories you have specified with -I options (and the directory of the current file, if appropriate) are searched.
-std=standard: Specify the standard to which the code should conform. For example, -std=c99 for ISO C99 standard.
-E: Stop after the preprocessing stage; do not run the compiler proper. The output is sent to standard output.
*/

int main(int argc, char *argv[])
{
  int opt;
  // Define your supported options here. The colon after each letter indicates that the option requires an argument.
  const char *optString = "D:U:I:include:nostdinc:std:E";

  if (initsearchdirs() != 0) {
    return 1;
  } 
  while ((opt = getopt(argc, argv, optString)) != -1) {
    switch (opt) {
      case 'D':
        printf("Define macro: %s\n", optarg);
        // Here you would add code to define a macro
        break;
      case 'U':
        printf("Undefine macro: %s\n", optarg);
        // Here you would add code to undefine a macro
        break;
      case 'I':
        printf("Include path: %s\n", optarg);
        addsearchdir(optarg);
        break;
      case 'E':
        printf("Stop after preprocessing\n");
        // Here you would add code to stop after preprocessing
        break;
      // Handle other options, including 'include', 'nostdinc', and 'std'
      default:
        // Handle unknown options and missing option arguments
        printf("Unknown option or missing option argument: %c\n", opt);
        break;
    }
  }

  if (newinstream("src/main.c", 1) != 0) {
    return 1;
  }

  char buf[4096];
  int rtn;
  while ((rtn = readline(NULL, buf, sizeof(buf))) == 0) {
    if (iscmdline(buf)) {
      if (processcmdline(buf, sizeof(buf)) != 0) {
        printf("Error processing command line\n");
        instream_t *in = getcurrentinstream();
        printf("%s(%d, %d): %s\n", in->fname, in->line, in->col, strerror(in->error));
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

  printMacroList();

  return 0;
}
