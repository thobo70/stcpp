/**
 * @file main.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Main entry point for the Super Tiny C Preprocessor (stcpp).
 * @version 0.1
 * @date 2024-08-07
 *
 * @copyright Copyright (c) 2024
 *
 * This file contains the main function and command-line processing logic
 * for the Super Tiny C Preprocessor. It handles command-line arguments,
 * initializes the preprocessor, and processes input files.
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
#include "version.h"

/**
 * @brief Prints usage information and help text.
 * 
 * Displays comprehensive help information including command-line options,
 * usage examples, and supported features of the preprocessor.
 * 
 * @param program_name Name of the program executable.
 */
void print_help(const char *program_name) {
  printf("Usage: %s [-Dname[=value]] [-Uname] [-Ipath] [-h] [-v|--version] infile outfile\n\n", program_name);
  printf("stcpp - Super Tiny C Preprocessor %s\n", get_version_string());
  printf("A lightweight C preprocessor with comprehensive macro support.\n\n");
  printf("Options:\n");
  printf("  -Dname[=value]  Define a macro with optional value\n");
  printf("  -Uname          Undefine a macro\n");
  printf("  -Ipath          Add directory to include search path\n");
  printf("  -h              Show this help message and exit\n");
  printf("  -v, --version   Show version information and exit\n\n");
  printf("Arguments:\n");
  printf("  infile          Input C source file to preprocess (use '-' for stdin)\n");
  printf("  outfile         Output file for preprocessed code (use '-' for stdout)\n\n");
  printf("Examples:\n");
  printf("  %s input.c output.c\n", program_name);
  printf("  %s -DDEBUG=1 -Iinclude input.c output.c\n", program_name);
  printf("  %s -DVERSION=2 -UOLDFEATURE input.c -\n", program_name);
  printf("  echo '#define TEST 42' | %s - -\n", program_name);
}

/**
 * @brief Main entry point for the Super Tiny C Preprocessor.
 * 
 * Processes command-line arguments, initializes the preprocessor subsystems,
 * opens input and output files, and performs preprocessing. Supports:
 * - Macro definition and undefinition (-D, -U options)
 * - Include path specification (-I option)
 * - File inclusion processing
 * - Conditional compilation (#if, #ifdef, etc.)
 * - Built-in macro expansion (__LINE__, __FILE__)
 * - Line directive processing (#line)
 * 
 * The preprocessor reads input line by line, processing preprocessor
 * directives and expanding macros in regular source lines.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return 0 on success, 1 on error.
 */

int main(int argc, char *argv[])
{
  int opt;
  FILE *outfile = stdout;
  char *outfname = NULL, *infname = NULL;
  // Define your supported options here. The colon after each letter indicates that the option requires an argument.
  const char *optString = "D:U:I:hv";

  // Check for --version before getopt processing
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--version") == 0) {
      print_version_info();
      return 0;
    }
  }

  if (initsearchdirs() != 0) {
    return 1;
  }

  while ((opt = getopt(argc, argv, optString)) != -1) {
    switch (opt) {
      case 'D':
        // cppcheck-suppress syntaxError
        DPRINT("Define macro: %s\n", optarg);
        if (addCommandLineMacro(optarg) != 0) {
          fprintf(stderr, "Error: Invalid macro definition: %s\n", optarg);
          return 1;
        }
        break;
      case 'U':
        DPRINT("Undefine macro: %s\n", optarg);
        if (banMacro(optarg) != 0) {
          fprintf(stderr, "Error: Cannot ban macro '%s'\n", optarg);
          return 1;
        }
        break;
      case 'I':
        addsearchdir(optarg);
        break;
      case 'h':
        print_help(argv[0]);
        return 0;
      case 'v':
        print_version_info();
        return 0;
     default:
        // Handle unknown options and missing option arguments
        fprintf(stderr, "Unknown option or missing option argument: %c\n", opt);
        break;
    }
  }

  if (optind != argc - 2) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "cpp [-Dname[=value]] [-Uname] [-Ipath] [-h] [-v|--version] infile outfile\n");
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

  char buf[4096];
  int rtn;
  while ((rtn = readline(NULL, buf, sizeof(buf))) == 0) {
    instream_t *in = getcurrentinstream();
    if (in->eof) {
      break;
    }
    if (iscmdline(buf)) {
      if (processcmdline(buf, sizeof(buf), outfile) != 0) {
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
  if (rtn < 0 && getcurrentinstream() != NULL) {
    printf("Error reading file\n");
  }
  // printf("%s: %s\n", in.fname, strerror(in.error));

#ifndef NDEBUG
  printMacroList();
#endif

  return 0;
}
