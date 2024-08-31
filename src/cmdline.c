/**
 * @file cmdline.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#define NDEBUG
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "debug.h"
#include "cmdline.h"
#include "input.h"
#include "macro.h"
#include "exprint.h"



typedef enum cmdtoken {
  EMPTY,
  INCLUDE,
  DEFINE,
  UNDEF,
  IF,
  IFDEF,
  IFNDEF,
  ELSE,
  ELIF,
  ENDIF,
  ERROR,
  PRAGMA,
  LINE,
  UNKNOWN,
  Err
} cmdtoken_t;

const char *cmdnames[] = {
  "",
  "include",
  "define",
  "undef",
  "if",
  "ifdef",
  "ifndef",
  "else",
  "elif",
  "endif",
  "error",
  "pragma",
  "line"
};


typedef struct cmdcond {
  cmdcondstate_t state;
  int ifstate;            // 1 if the condition is true, 0 otherwise
  struct cmdcond *prev;   // previous condition
} cmdcond_t;


cmdcond_t *cmdcond = NULL;
int condstate = 1;

/**
 * @brief Check if a line is a command line.
 * 
 * This function checks if a line is a command line by looking at the first character.
 * If the first character is a '#', it returns 1, otherwise it returns 0.
 * 
 * @param line The line to check.
 * @return 1 if the line is a command line, 0 otherwise.
 */
int iscmdline(char *line)
{  // NOLINT
  assert(line != NULL);

  if (line[0] == '#') {
    return 1;
  }
  return 0;
}



/**
 * @brief Get the type of a command.
 * 
 * This function takes a command string as input and returns its type as an integer.
 * The type is determined by the position of the command in the cmdnames array.
 * If the command is not found in the array, it returns UNKNOWN.
 * If the command is NULL, it returns Err.
 * 
 * @param cmd The command string.
 * @return The type of the command as an cmdtoken_t, or UNKNOWN if the command is not found, or Err if the command is NULL.
 */
cmdtoken_t getcmdtype(char *cmd)
{
  if (cmd == NULL) {
    return Err;
  }
  for (int i = 0; i < (int)(sizeof(cmdnames) / sizeof(cmdnames[0])); i++) {
    if (strcmp(cmd, cmdnames[i]) == 0) {
      return i;
    }
  }
  return UNKNOWN;
}



int check_defined(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);

  char *strend = buf + strlen(buf) + 1;
  char *start = buf;
  char *macro_start, *macro_end, *defined_end;
  char replace[2];  // Buffer to hold the ASCII number

  if (buf >= end || strend >= end) {
    fprintf(stderr, "check_defined: buffer overflow\n");
    return -1;  // error
  }
  while (start < strend)
  {
    int pmode = 0;
    // Find the start of a "defined" expression
    if ((start = strstr(start, "defined")) != NULL)
    {
      buf = start + 7;  // Move past "defined" keyword
      while (isspace(*buf)) ++buf;  // Skip whitespace

      if (*buf == '(') {  // If macro is in parentheses
        macro_start = buf + 1;  // Move past "(" to the start of the macro name
        defined_end = strchr(macro_start, ')');  // Find the end of the macro name
        if (defined_end == NULL) {
          fprintf(stderr, "check_defined: missing ')'\n");
          return -1;
        }
        pmode = 1;
      } else {  // If macro is not in parentheses
        macro_start = buf;
        defined_end = strend;
      }

      while (isspace(*macro_start)) ++macro_start;  // Skip whitespace

      buf = macro_start;
      while (buf < defined_end) {
        if (!isIdent(*buf, buf - macro_start)) {
          break;
        }
        ++buf;
      }
      if (buf == macro_start) {
        fprintf(stderr, "check_defined: missing/wrong macro name\n");
        return -1;
      }
      macro_end = buf;  // Move back to the end of the macro name
      if (pmode == 0) {
        defined_end = buf;
      }

      // Check if the macro is defined and get the ASCII number
      replace[0] = isdefinedMacro(macro_start, macro_end) + '0';
      replace[1] = '\0';

      // Replace the "defined(macro)" expression with the ASCII number
      replaceBuf(start, defined_end, strend, replace);

      // Update strend in case the buffer size has changed
      strend = start + strlen(start) + 1;

      // Move past the "defined(macro)" expression replacement
      start++;
    } else {
      // No more "defined(macro)" expressions
      break;
    }
  }

  DPRINT("check_defined ok\n");
  return 0;
}



void stripspaces(char *buf)
{
  assert(buf != NULL);

  char *dest = buf;
  while (*buf != '\0') {
    if (isspace(*buf)) {
      buf++;
    } else {
      *dest++ = *buf++;
    }
  }
  *dest = '\0';
}



int evalifexpr(char *buf, char *end, result_t *result)
{
  assert(buf != NULL);
  assert(end != NULL);
  assert(result != NULL);
  *result = 0;

  DPRINT("ifEvalpre: %s\n", buf);
  stripspaces(buf);
  if (check_defined(buf, end) != 0 || processBuffer(buf, end - buf, 1) != 0) {
    return -1;
  }
  stripspaces(buf);
  DPRINT("ifEvalpost: %s\n", buf);

  *result = evaluate_expression(buf);
  if (expr_error != EE_OK) {
    DPRINT("Error evaluating if expression %d\n", expr_error);
    return -1;
  }

  DPRINT("ifEvalResult: %ld\n", *result);

  return 0;
}



int do_include(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);
  char *strend = buf + strlen(buf) + 1;
  char *fname_start, *fname_end;
  int flag = 0;

  if (buf >= end || strend >= end) {
    return -1;  // error
  }

  // Find the start of the filename
  if ((fname_start = strchr(buf, '<')) != NULL) {
    fname_start++;
    fname_end = strchr(fname_start, '>');
  } else if ((fname_start = strchr(buf, '\"')) != NULL) {
    fname_start++;
    fname_end = strchr(fname_start, '\"');
    flag = 1;
  } else {
    return -1;
  }
  if (fname_end == NULL) {
    return -1;
  }

  // Copy the filename to a buffer
  *fname_end = '\0';
  if (newinstream(fname_start, flag) != 0) {
    return -1;
  }
  return 0;
}



/**
 * @brief processes a cpp command line
 * 
 * It expects to get the whole line in buf, including the '#'
 * 
 * @param buf 
 * @param size 
 * @return int 
 */
int processcmdline(char *buf, int size)
{
  assert(buf != NULL);
  assert(size > 0);
  char *end = buf + size - 1;
  char *start = ++buf;  // skip the '#'
  char *strend = start + strlen(start);
  result_t result;
  static int ifdepth = 0;  // depth of nested if statements in case of ignored commands

  if (buf >= end || strend >= end) {
    return -1;  // error
  }

  if (*buf == ' ') {    // skip leading whitespaces
    ++buf;
    ++start;
  }

  while (buf < end && *buf != ' ' && *buf != '\n' && *buf != '\0') {  // find the end of the command
    ++buf;
  }
  *buf = '\0';

  cmdtoken_t cmd = getcmdtype(start);
  if (cmd == Err) {
    return -1;
  }
  if (cmdcond != NULL) {
    if (condstate == 0) {
      if (cmd == IF || cmd == IFDEF || cmd == IFNDEF) {
        DPRINT("Ignoring if statement\n");
        ifdepth++;
      } else if (cmd == ELSE) {
        if (ifdepth > 0) {
          DPRINT("Ignoring else statement\n");
        } else {
          DPRINT("Else: %d\n", cmdcond->ifstate);
          condstate = !cmdcond->ifstate;
          cmdcond->state = COND_ELSE;
        }
      } else if (cmd == ENDIF) {
        if (ifdepth > 0) {
          DPRINT("Ignoring endif statement\n");
          ifdepth--;
        } else {
          DPRINT("Endif: %d\n", cmdcond->ifstate);
          cmdcond_t *tmp = cmdcond;
          cmdcond = cmdcond->prev;
          free(tmp);
          condstate = 1;
        }
      }
      return 0;
    }
    if (cmdcond->state == COND_IF) {
      if (cmd == ELIF) {
        if (evalifexpr(buf + 1, end, &result) != 0) {
          DPRINT("Error evaluating if expression\n");
          return -1;
        }
        condstate = cmdcond->ifstate = result;
        DPRINT("Elif: %d\n", cmdcond->ifstate);
      } else if (cmd == ELSE) {
        condstate = !cmdcond->ifstate;
        cmdcond->state = COND_ELSE;
        DPRINT("Else: %d\n", cmdcond->ifstate);
      } else if (cmd == ENDIF) {
        DPRINT("Endif: %d\n", cmdcond->ifstate);
        cmdcond_t *tmp = cmdcond;
        cmdcond = cmdcond->prev;
        free(tmp);
        condstate = 1;
      }
    } else if (cmdcond->state == COND_ELSE) {
      if (cmd == ENDIF) {
        cmdcond_t *tmp = cmdcond;
        cmdcond = cmdcond->prev;
        free(tmp);
        condstate = 1;
      }
      if (cmd == ELIF || cmd == ELSE) {
        DPRINT("Error: unexpected else or elif\n");
        return -1;
      }
    }
    if (condstate == 0) {
      return 0;
    }
  }
  switch (cmd) {
    case EMPTY:
      DPRINT("empty cmd\n");
      break;
    case INCLUDE:
      if (processBuffer(buf, end - buf, 0) != 0) {
        return -1;
      }
      DPRINT("Include: %s\n", buf + 1);
      if (do_include(buf + 1, end) != 0) {
        return -1;
      }
      break;
    case DEFINE:
      DPRINT("Define: %s\n", buf + 1);
      addMacro(buf + 1);
      // printMacroList();  // @todo remove
      break;
    case UNDEF:
      DPRINT("Undef: %s\n", buf + 1);
      deleteMacro(buf + 1);
      break;
    case IF:
    {
      cmdcond_t *tmp = malloc(sizeof(cmdcond_t));
      tmp->state = COND_IF;
      DPRINT("If: %s\n", buf + 1);
      if (evalifexpr(buf + 1, end, &result) != 0) {
        return -1;
      }
      tmp->ifstate = result;
      tmp->prev = cmdcond;
      cmdcond = tmp;
      condstate = tmp->ifstate;
      ifdepth = 0;
      break;
    }
    case IFDEF:
    {
      cmdcond_t *tmp = malloc(sizeof(cmdcond_t));
      tmp->state = COND_IF;
      tmp->ifstate = isdefinedMacro(buf + 1, buf + 1 + strlen(buf + 1));
      tmp->prev = cmdcond;
      cmdcond = tmp;
      condstate = tmp->ifstate;
      ifdepth = 0;
      DPRINT("Ifdef: %s %d\n", buf + 1, tmp->ifstate);
      break;
    }
    case IFNDEF:
    {
      cmdcond_t *tmp = malloc(sizeof(cmdcond_t));
      tmp->state = COND_IF;
      tmp->ifstate = !isdefinedMacro(buf + 1, buf + 1 + strlen(buf + 1));
      tmp->prev = cmdcond;
      cmdcond = tmp;
      condstate = tmp->ifstate;
      ifdepth = 0;
      DPRINT("Ifndef: %s %d\n", buf + 1, tmp->ifstate);
      break;
    }
    case ELSE:
      DPRINT("Else:\n");
      break;
    case ELIF:
      DPRINT("Elif: %s\n", buf + 1);
      break;
    case ENDIF:
      DPRINT("Endif:\n");
      break;
    case ERROR:
      DPRINT("Error: %s\n", buf + 1);
      break;
    case PRAGMA:
      DPRINT("Pragma: %s\n", buf + 1);
      break;
    case LINE:
      DPRINT("Line: %s\n", buf + 1);
      break;
    case UNKNOWN:
      DPRINT("Unknown: %s\n", buf + 1);
      break;
    default:
      DPRINT("default: %s\n", buf + 1);
      break;
  }
  return 0;
}



