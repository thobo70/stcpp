/**
 * @file macro.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief This file contains the implementation of macros.
 * @version 0.1
 * @date 2024-03-30
 * @copyright Copyright (c) 2024
 *
 * The macro.c file contains a set of functions that implement a basic macro
 * processor. The macro processor allows you to define macros, which are named
 * chunks of code, and then replace instances of those macros in your code with
 * their definitions. Here's a detailed description of how the functions
 * interact:
 *
 * addMacro(char *buf): This function is used to add a new macro to the macro
 * list. It parses the input buffer to extract the macro name, parameters (if
 * any), and replacement text. If the macro list is empty, the new Macro node
 * becomes the head of the list. Otherwise, the new Macro node is appended to
 * the end of the list.
 *
 * deleteMacro(char *name): This function is used to delete a macro from the
 * macro list. It searches the macro list for a macro with the given name and
 * removes it from the list.
 *
 * findMacro(char *start, char *end): This function is used to find a macro in
 * the macro list. It searches the macro list for a macro with the given name.
 *
 * isdefinedMacro(char *start, char *end): This function checks if a macro is
 * defined. It uses the findMacro function to search for the macro in the macro
 * list. If the macro is found, it returns 1; otherwise, it returns 0.
 *
 * replaceBuf(char *start, char *buf, char *end, char *replace): This function
 * is used to replace a portion of a buffer with a given string. It replaces
 * the portion of the buffer starting at buf and ending at the end of the
 * string in the buffer with the replacement string.
 *
 * removeDoubleHash(char *buf, char *endmacro): This function is used to remove
 * double hash (##) tokens from a macro. It scans the buffer for double hash (##)
 * tokens and removes them by shifting the remaining contents of the buffer to
 * the left.
 *
 * findEndOfParameter(char *buf, char *end): This function is used to find the
 * end of a parameter in a buffer. It iterates over the buffer until it finds a
 * comma or a closing parenthesis, which signifies the end of a parameter.
 *
 * processMacro(char *buf, int len): This function is used to process a macro in
 * a buffer. It scans the buffer for a macro and replaces it with its replacement
 * text. If the macro is a functional macro, it also replaces the parameters in
 * the replacement text with their corresponding arguments.
 *
 * processBuffer(char *buf, int len): This function is used to process a buffer
 * to recognize and replace macros. It scans the buffer for macros and replaces
 * them with their replacement text. If a macro is a functional macro, it also
 * replaces the parameters in the replacement text with their corresponding
 * arguments.
 *
 * The other functions in the file are helper functions that are used to skip
 * over whitespace, strings, and expressions in a buffer, check if a character
 * is a valid identifier character, and print the list of macros.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debug.h"
#include "macro.h"

/**
 * @struct MacroParam
 * @brief A structure to represent a parameter of a macro.
 * 
 * This structure is used to store the name of a parameter of a macro.
 * It also contains a pointer to the next parameter in the list, allowing
 * the parameters to be stored in a linked list.
 */
typedef struct macroparam {
    struct macroparam *next; /**< Pointer to the next parameter in the list. */
    char *name; /**< Name of the parameter. */
} MacroParam;



/**
 * @struct Macro
 * @brief A structure to represent a macro.
 * 
 * This structure is used to store the details of a macro, including its name,
 * parameters, and replacement text. It also contains a pointer to the next macro
 * in the list, allowing the macros to be stored in a linked list.
 */
typedef struct macro {
    struct macro *next;    /**< Pointer to the next macro in the list, or NULL if it is the last one */
    char *name;            /**< Name of the macro, or NULL if it is a simple macro */
    MacroParam *param;     /**< Pointer to the list of parameters of the macro, or NULL if there is no parameter */
    char *replace;         /**< Replacement text of the macro. */
} Macro;



Macro *macroList = NULL;



/**
 * @brief Skips over whitespace characters in a buffer.
 * 
 * This function takes a pointer to a buffer and a pointer to the end of the buffer.
 * It advances the buffer pointer past any leading whitespace characters and returns
 * the updated pointer.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param end Pointer to the end of the buffer.
 * @return Pointer to the first non-whitespace character in the buffer, or the end of the buffer if no such character exists.
 */
char *skipSpaces(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);
  while (buf < end && isspace(*buf)) {
    buf++;
  }
  return buf;
}



/**
 * @brief Skips over a string in a buffer.
 * 
 * This function takes a pointer to a buffer and a pointer to the end of the buffer.
 * It advances the buffer pointer past a string. A string is considered to be any sequence
 * of characters enclosed in double quotes (""). If a double quote is preceded by a backslash (\"),
 * it is considered to be part of the string rather than the end of the string.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param end Pointer to the end of the buffer.
 * @return Pointer to the character immediately after the end of the string, or the end of the buffer if no such character exists.
 */
char *skipString(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);
  do {
    buf++;
  } while (buf < end && !(*buf == '\"' && *(buf - 1) != '\\'));
  return buf;
}



/**
 * @brief Skips over an expression in a buffer.
 * 
 * This function takes a pointer to a buffer and a pointer to the end of the buffer.
 * It advances the buffer pointer past an expression. An expression is considered to be
 * any sequence of characters enclosed in parentheses (()). Nested expressions are handled
 * correctly. If an expression contains a string (a sequence of characters enclosed in double
 * quotes ("")), the string is skipped over using the skipString function.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param end Pointer to the end of the buffer.
 * @return Pointer to the character immediately after the end of the expression, or the end of the buffer if no such character exists.
 */
char *skipExpression(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);
  int count = 1;
  while (buf < end && count > 0) {
    switch (*++buf) {
      case '(':
        count++;
        break;
      case ')':
        count--;
        break;
      case '\"':
        buf = skipString(buf, end);
        break;
    }
  }
  return buf;
}



/**
 * @brief Prints the list of macros.
 *
 * This function prints the list of macros stored in the `macroList` data structure.
 * It iterates through each macro in the list and prints its name, parameters (if any),
 * and replacement text (if any).
 */
void printMacroList()
{
  Macro *temp = macroList;
  DPRINT("*** Macro List:\n");
  while (temp != NULL) {
    DPRINT("%s", temp->name);
    MacroParam *param = temp->param;
    if (param != NULL)
      DPRINT("(");
    while (param != NULL) {
      if (param->name != NULL)
        DPRINT("%s", param->name);
      param = param->next;
      if (param != NULL) {
        DPRINT(", ");
      } else {
        DPRINT(")");
      }
    }
    if (temp->replace != NULL) {
      DPRINT(" -> %s\n", temp->replace);
    } else {
      DPRINT("\n");
    }
    temp = temp->next;
  }
  DPRINT("*** EOL\n");
}



/**
 * @brief Checks if a character is a valid identifier character.
 *
 * This function checks whether the given character is a valid identifier character.
 * An identifier character can be an alphabet (uppercase or lowercase), a digit, or an underscore.
 *
 * @param c The character to be checked.
 * @param idx The index of the character in the string.
 * @return Returns 1 if the character is a valid identifier character, 0 otherwise.
 */
int isIdent(char c, int idx)
{
  if (idx < 0 || c == '\0') {
    return 0;
  }

  return idx > 0 ? (isalnum(c) || c == '_') : (isalpha(c) || c == '_');
}



/**
 * @brief Parses the input buffer to extract the macro name, parameters (if any), and replacement text.
 *
 * This function parses the input buffer to extract the macro name, parameters (if any), and replacement text.
 * It starts by finding the beginning of the macro name and then searches for the end of the macro name,
 * which must be a valid identifier followed by a '(' or a space. If the macro name is not valid or if there
 * is no '(' or space after it, the function returns -1 to indicate an error.
 *
 * If a '(' is found after the macro name, the function proceeds to parse the parameter list. It iterates through
 * the buffer, extracting each parameter name and adding it to the parameter list. The parameter names must be valid
 * identifiers separated by commas. If a ')' is found, the parameter list is complete. If a ',' is found, the function
 * continues parsing for the next parameter name. If any error occurs during the parsing of the parameter list,
 * the function returns -1 to indicate an error.
 * If the parameter list is empty, the function creates a single MacroParam node with a NULL name to represent
 * a functional macro without parameters. In case no '(' is found after the macro name, the function creates no
 * MacroParam node to represent a simple macro without parameters, param is NULL.
 *
 * After parsing the parameter list, the function removes any preceding spaces and extracts the replacement text.
 * The replacement text is the remaining part of the buffer after the parameter list. If the replacement text is not
 * empty, it is stored in the newly created Macro node. If the buffer is too small to store the replacement text,
 * the function returns -1 to indicate an error.
 *
 * Finally, the function creates a new Macro node, initializes its fields with the parsed information, and adds it
 * to the macro list. If the macro list is empty, the new Macro node becomes the head of the list. Otherwise, the
 * new Macro node is appended to the end of the list.
 *
 * @param buf The input buffer containing the macro definition.
 * @return 0 if the macro was successfully added, -1 if there was an error.
 *
 * @startuml
 * 
 * start
 * :Find beginning of macro name;
 * :Search for end of macro name;
 * if (Macro name is not valid or no '(' or space after it) then
 *   :Return -1;
 * endif
 * if (A '(' is found after the macro name) then
 *   :Parse parameter list;
 *   if (Error occurs during parsing) then
 *     :Return -1;
 *   endif
 * endif
 * :Remove preceding spaces;
 * :Extract replacement text;
 * if (Buffer is too small to store replacement text) then
 *   :Return -1;
 * endif
 * :Create new Macro node;
 * :Initialize fields with parsed information;
 * if (Macro list is empty) then
 *   :New Macro node becomes head of list;
 * else
 *   :Append new Macro node to end of list;
 * endif
 * :Return 0;
 *
 * @enduml
 */
int addMacro(char *buf)
{
  char *token, *end = buf + strlen(buf), *name = NULL;

  // find begin of macro name
  while (buf < end && isspace(*buf)) {
    buf++;
  }

  // find end of macro name, has to be a valid identifier and there has to be a '(' or ' ' after it
  name = buf;
  int i = 0;
  while (buf < end && isIdent(*buf, i++)) {
    buf++;
  }
  char type = *buf;
  *buf = '\0';
  if (*name == '\0' || strchr(" (", type) == NULL) {
    return -1;    /** @todo  error no macro */
  }

  // check for parameter list if a '(' is found
  MacroParam *paramList = NULL;
  if (type == '(') {
    buf++;
    MacroParam *param = NULL;
    while (buf < end) {
      if (param == NULL) {
        param = malloc(sizeof(MacroParam));
        paramList = param;
      } else {
        param->next = malloc(sizeof(MacroParam));
        param = param->next;
      }
      param->next = NULL;
      param->name = NULL;
      // remove preciding spaces
      while (buf < end && isspace(*buf)) {
        buf++;
      }
      // find end of parameter name, has to be a valid identifier
      i = 0;
      token = buf;
      while (buf < end && isIdent(*buf, i++)) {
        buf++;
      }
      if (buf >= end) {
        return -1;    /** @todo error no ')' */
      }
      char c = *buf;
      *buf++ = '\0';
      if (isspace(c)) {
        // remove trailing spaces
        while (buf < end && isspace(*buf)) {
          buf++;
        }
        if (buf >= end) {
          return -1;    /** @todo error no ')' */
        }
        c = *buf;
        buf++;
      }
      if (*token != '\0')
        param->name = strdup(token);
      if (c == ')') {
        break;
      }
      if (c != ',') {
        return -1;    /** @todo  error no ',' */
      }
    }
  }

  // remove preciding spaces before the replacement text
  buf = skipSpaces(buf, end);

  // Create a new Macro node
  Macro *newMacro = malloc(sizeof(Macro));
  newMacro->next = NULL;
  newMacro->name = strdup(name);
  newMacro->param = paramList;
  if (*buf != '\0') {
    newMacro->replace = strdup(buf);
  } else {
    newMacro->replace = NULL;
  }

  // Add the new macro to the macro list
  if (macroList == NULL) {
    macroList = newMacro;
  } else {
    Macro *temp = macroList;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newMacro;
  }

  return 0;
}



/**
 * @brief Deletes a macro from the macro list.
 * 
 * This function takes the name of a macro and deletes it from the macro list.
 * It frees all memory associated with the macro, including the memory for the
 * macro's name, replacement text, and parameters. If the macro is not found in
 * the list, it returns -1.
 * 
 * @param name Name of the macro to delete.
 * @return 0 if the macro was successfully deleted, -1 if the macro was not found.
 * 
 * @startuml
 * start
 * :Initialize temp to macroList;
 * :Initialize prev to NULL;
 * while (temp is not NULL)
 *   :Compare temp->name with name;
 *   if (temp->name equals name) then (yes)
 *     :Remove temp from list;
 *     :Free temp->name;
 *     :Free temp->replace;
 *     while (param is not NULL)
 *       :Free param->name;
 *       :Move to next param;
 *     endwhile
 *     :Free temp;
 *     :Return 0;
 *     stop
 *   else (no)
 *     :Move to next macro;
 *   endif
 * endwhile
 * :Return -1;
 * stop
 * @enduml
 */
int deleteMacro(char *name)
{
  Macro *temp = macroList, *prev = NULL;

  while (temp != NULL) {
    if (strcmp(temp->name, name) == 0) {
      if (prev == NULL) {
        macroList = temp->next;
      } else {
        prev->next = temp->next;
      }
      free(temp->name);
      free(temp->replace);
      MacroParam *param = temp->param;
      while (param != NULL) {
        if (param->name != NULL)
          free(param->name);
        MacroParam *next = param->next;
        free(param);
        param = next;
      }
      free(temp);
      return 0;
    }
    prev = temp;
    temp = temp->next;
  }
  return -1;
}



/**
 * @brief Finds a macro in the macro list.
 *
 * This function searches the macro list for a macro with the given name.
 *
 * @param start The start of the name.
 * @param end The end of the name.
 * @return A pointer to the Macro node if the macro is found, NULL otherwise.
 */
Macro *findMacro(char *start, char *end)
{
  Macro *temp = macroList;

  while (temp != NULL) {
    if (strlen(temp->name) == (size_t)(end - start) && strncmp(temp->name, start, strlen(temp->name)) == 0) {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}



/**
 * @brief Checks if a macro is defined.
 * 
 * This function takes a pointer to the start and end of a macro name in a buffer.
 * It uses the findMacro function to search for the macro in the macro list.
 * If the macro is found, it returns 1; otherwise, it returns 0.
 * 
 * @param start Pointer to the start of the macro name in the buffer.
 * @param end Pointer to the end of the macro name in the buffer.
 * @return 1 if the macro is defined, 0 otherwise.
 */
int isdefinedMacro(char *start, char *end)
{
  return findMacro(start, end) != NULL;
}



/**
 * @brief Replaces a portion of a buffer with a given string.
 * 
 * This function takes pointers to the start and end of a buffer, a pointer to the portion
 * of the buffer to be replaced, and a replacement string. It replaces the portion of the
 * buffer starting at `buf` and ending at the end of the string in the buffer with the
 * replacement string. If the buffer is too small to hold the replacement string and the
 * remaining contents of the buffer, it returns NULL.
 * 
 * @param start Pointer to the start of the buffer.
 * @param buf Pointer to the portion of the buffer to be replaced.
 * @param end Pointer to the end of the buffer.
 * @param replace The replacement string.
 * @return Pointer to the character immediately after the replacement string in the buffer, or NULL if the buffer is too small.
 */
char *replaceBuf(char *start, char *buf, char *end, char *replace)
{
  char * endstr = buf + strlen(buf) + 1;
  if (replace == NULL) {
    replace = "";
  }
  int rlen = strlen(replace);

  if (start + rlen + (endstr - buf) >= end) {  // buffer too small
    return NULL;
  }

  memmove(start + rlen, buf, endstr - buf);
  memcpy(start, replace, rlen);

  return start + rlen;
}



/**
 * @brief Removes double hash (##) tokens from a macro.
 * 
 * This function takes a pointer to a buffer and a pointer to the end of the macro in the buffer.
 * It scans the buffer for double hash (##) tokens and removes them by shifting the remaining
 * contents of the buffer to the left. The end of the string in the buffer is updated accordingly.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param endmacro Pointer to the end of the macro in the buffer.
 */
void removeDoubleHash(char *buf, char *endmacro)
{
  char *endstr = endmacro + strlen(endmacro);

  while (buf < endmacro) {
    if (*buf == '#' && *(buf + 1) == '#') {
      // remove both '#'
      memmove(buf, buf + 2, endstr - buf - 2);
      endstr -= 2;
      endmacro -= 2;
    } else {
      buf++;
    }
  }
}



/**
 * @brief Finds the end of a parameter in a buffer.
 * 
 * This function iterates over the buffer until it finds a comma or a closing parenthesis,
 * which signifies the end of a parameter. It also handles nested functional macros,
 * other expressions, and strings.
 * 
 * @param buf Pointer to the start of the parameter.
 * @param end Pointer to the end of the buffer.
 * @return Pointer to the end of the parameter.
 */
char *findEndOfParameter(char *buf, char *end) {
  while (buf < end && *buf != ',' && *buf != ')') {  // find end of parameter
    if (*buf == '\"')  // skip string
      buf = skipString(buf, end);
    if (*buf == '(')  // nested functional macro or other expressions or strings
      buf = skipExpression(buf, end);
    buf++;
  }
  return buf;
}



/**
 * @brief Processes a macro in a buffer.
 * 
 * This function takes a pointer to a buffer and the length of the buffer.
 * It scans the buffer for a macro and replaces it with its replacement text.
 * If the macro is a functional macro, it also replaces the parameters in the
 * replacement text with their corresponding arguments. If the buffer is too small
 * to hold the replacement text and the remaining contents of the buffer, it returns -1.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param len Length of the buffer.
 * @return 0 if the macro was successfully processed, -1 if an error occurred, or the number of characters processed if no macro was found.
 */
int processMacro(char *buf, int len)
{
  char *start = buf, *end = buf + len;
  Macro *parammacrolist = NULL;

  while (buf <= end && isIdent(*buf, buf - start)) {
    buf++;
  }
  if (buf == start) {  // no valid identifier, therefore advance one char
    return 1;
  }
  Macro *macro = findMacro(start, buf);
  if (macro == NULL) {  // no macro found
    return buf - start;
  }

  MacroParam *param = macro->param;
  if (param != NULL) {  // functional macro
    Macro *parammacro = NULL;
    // check for '(', there are no white spaces allowed
    if (*buf != '(') {
      return -1;
    }
    buf++;
    // int n = 0;
    while (param != NULL) {
      buf = skipSpaces(buf, end);
      if (buf >= end) {
        return -1;
      }
      char *paramstart = buf;
      buf = findEndOfParameter(buf, end);
      if (buf >= end) {
        return -1;
      }

      if (*buf == ')') {
        if (param->next != NULL) {  // error not enough parameters
          return -1;
        }
        if (param->name == NULL) {  // functional macro without parameter
          if (buf == paramstart) {
            break;
          } else {
            return -1;  // error extra parameter in functional macro not expected
          }
        } else {
          if (param->next != NULL) {  // error not enough parameters
            return -1;
          }
        }
      } else if (*buf == ',') {
        if (param->next == NULL) {  // error too many parameters
          return -1;
        }
      }
      if (parammacro == NULL) {
        parammacro = malloc(sizeof(Macro));
        parammacrolist = parammacro;
      } else {
        parammacro->next = malloc(sizeof(Macro));
        parammacro = parammacro->next;
      }
      parammacro->next = NULL;
      parammacro->name = param->name;
      parammacro->param = NULL;
      parammacro->replace = malloc(buf - paramstart + 1);
      memcpy(parammacro->replace, paramstart, buf - paramstart);
      parammacro->replace[buf - paramstart] = '\0';

      param = param->next;
      buf++;
    }
  }

  buf = replaceBuf(start, buf, end, macro->replace);
  if (buf == NULL) {  // buffer too small
    return -1;
  }

  if (parammacrolist != NULL) {
    char *token = start;
    while (token < buf) {
      if (isIdent(*token, token - start)) {
        char *tokenend = token;
        while (tokenend < buf && isIdent(*tokenend, tokenend - token)) {
          tokenend++;
        }
        Macro *parammacro = parammacrolist;
        while (parammacro != NULL) {
          if (strlen(parammacro->name) == (size_t)(tokenend - token) && strncmp(parammacro->name, token, strlen(parammacro->name)) == 0) {
            char *newtoken = replaceBuf(token, token + strlen(parammacro->name), end, parammacro->replace);
            if (newtoken == NULL) {  // buffer too small
              return -1;
            }
            buf += newtoken - token;
            token = newtoken;
            break;
          }
          parammacro = parammacro->next;
        }
      }
      token++;
    }

    for (Macro *parammacro = parammacrolist, *next; parammacro != NULL; parammacro = next) {  // free memory of parammacro's
      next = parammacro->next;
      free(parammacro->replace);
      free(parammacro);
    }
  }

  removeDoubleHash(start, buf);

  return 0;
}



/**
 * @brief Processes a buffer to recognize and replace macros.
 * 
 * This function takes a pointer to a buffer and the length of the buffer.
 * It scans the buffer for macros and replaces them with their replacement text.
 * If a macro is a functional macro, it also replaces the parameters in the
 * replacement text with their corresponding arguments. If the buffer is too small
 * to hold the replacement text and the remaining contents of the buffer, it returns -1.
 * 
 * @param buf Pointer to the start of the buffer.
 * @param len Length of the buffer.
 * @return 0 if the buffer was successfully processed, -1 if an error occurred.
 */
int processBuffer(char *buf, int len)
{
  // Scan buf to recognize macros
  char *start = buf, *end = buf + len;

  while (buf < end && *buf != '\0') {
    buf = skipSpaces(buf, end);  // skip preceding spaces
    if (isIdent(*buf, 0)) {
      int cnt = processMacro(buf, end - buf);
      if (cnt < 0) {
        return cnt;
      }
      buf += cnt;
      continue;
    }
    // skip string
    if (*buf == '\"' && !(buf > start && *(buf - 1) == '\\'))
      buf = skipString(buf, end);
    // ignore char
    buf++;
  }
  return 0;
}
