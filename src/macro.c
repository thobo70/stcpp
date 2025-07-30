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
#define NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debug.h"
#include "macro.h"
#include "input.h"

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

/**
 * @struct BannedMacro
 * @brief A structure to represent a banned macro name.
 *
 * This structure is used to store the names of macros that should not be defined,
 * typically those specified with -U command line option.
 */
typedef struct bannedmacro {
    struct bannedmacro *next; /**< Pointer to the next banned macro in the list. */
    char *name; /**< Name of the banned macro. */
} BannedMacro;



Macro *macroList = NULL;
static int macro_expanded = 0;  // Flag to track if macro expansion occurred
static BannedMacro *bannedMacroList = NULL;  // List of banned macro names



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
  if (buf < end) {
    buf++;  // Skip past the closing quote
  }
  return buf;
}



char *skipNumber(char *buf, char *end)
{
  assert(buf != NULL);
  assert(end != NULL);
  if (!isdigit(*buf)) {
    return buf;
  }

  ++buf;
  while (buf < end) {
    char c = tolower(*buf);
    if (c == 'u' || c == 'l') {
      return ++buf;
    }
    if (!isdigit(c) && c != 'x' && c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f') {
      break;
    }
    buf++;
  }
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
  // cppcheck-suppress syntaxError
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

  // check if this macro is banned (from -U option)
  if (isMacroBanned(name)) {
    return 0;  // silently ignore banned macros
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
  } else {
    if (buf < end) {
      buf++;
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
 * @brief Parse and add a macro from command line -D option.
 *
 * This function parses command line macro definitions in the format:
 * - MACRO (defines MACRO as 1)
 * - MACRO=value (defines MACRO as value)
 * - MACRO= (defines MACRO as empty string)
 *
 * @param definition The macro definition string from -D option
 * @return 0 on success, -1 on error
 */
int addCommandLineMacro(const char *definition) {
    if (definition == NULL || *definition == '\0') {
        return -1;
    }

    // Find the '=' separator if it exists
    const char *equals = strchr(definition, '=');
    char *macro_string;

    if (equals == NULL) {
        // No '=' found, define as MACRO 1
        size_t len = strlen(definition);
        macro_string = malloc(len + 3);  // space for " 1\0"
        if (macro_string == NULL) {
            return -1;
        }
        strcpy(macro_string, definition);
        strcat(macro_string, " 1");
    } else {
        // '=' found, parse MACRO=value
        size_t name_len = equals - definition;
        size_t value_len = strlen(equals + 1);

        if (name_len == 0) {
            return -1;  // Empty macro name
        }

        macro_string = malloc(name_len + 1 + value_len + 1);  // space for "name value\0"
        if (macro_string == NULL) {
            return -1;
        }

        // Copy macro name
        strncpy(macro_string, definition, name_len);
        macro_string[name_len] = ' ';  // Separator

        // Copy macro value (can be empty)
        strcpy(macro_string + name_len + 1, equals + 1);
    }

    // Use existing addMacro function
    int result = addMacro(macro_string);
    free(macro_string);

    return result;
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
 * @brief Adds a macro name to the banned macros list.
 *
 * This function adds a macro name to the banned macros list, preventing
 * it from being defined later. This is used to implement the -U option.
 *
 * @param name Name of the macro to ban.
 * @return 0 on success, -1 on error.
 */
int banMacro(char *name)
{
  // First try to delete the macro if it already exists
  deleteMacro(name);
  
  // Add to banned list
  BannedMacro *newBanned = malloc(sizeof(BannedMacro));
  if (newBanned == NULL) {
    return -1;
  }
  
  newBanned->name = strdup(name);
  if (newBanned->name == NULL) {
    free(newBanned);
    return -1;
  }
  
  newBanned->next = bannedMacroList;
  bannedMacroList = newBanned;
  
  return 0;
}

/**
 * @brief Checks if a macro name is banned.
 *
 * This function checks if a macro name is in the banned macros list.
 *
 * @param name Name of the macro to check.
 * @return 1 if banned, 0 if not banned.
 */
int isMacroBanned(char *name)
{
  BannedMacro *temp = bannedMacroList;
  
  while (temp != NULL) {
    if (strcmp(temp->name, name) == 0) {
      return 1;
    }
    temp = temp->next;
  }
  return 0;
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

  // DPRINT("findMacro: %.*s\n", (int)(end - start), start);
  while (temp != NULL) {
    // DPRINT("findMacro: check %s\n", temp->name);
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
  char * endstr = start + strlen(start);  // Find end of entire string from start
  if (replace == NULL) {
    replace = "";
  }
  int rlen = strlen(replace);

  if (start + rlen + (endstr - buf) >= end) {  // buffer too small
    return NULL;
  }

  memmove(start + rlen, buf, endstr - buf + 1);  // +1 to include null terminator
  memcpy(start, replace, rlen);

  return start + rlen;
}



/**
 * @brief Creates a stringified version of a parameter value.
 *
 * This function takes a parameter value and converts it to a string literal
 * by adding quotes and escaping internal quotes and backslashes.
 *
 * @param value The parameter value to stringify.
 * @param result Buffer to store the stringified result.
 * @param result_size Size of the result buffer.
 * @return Length of the stringified result, or -1 if buffer too small.
 */
int stringifyParameter(const char *value, char *result, size_t result_size)
{
  if (value == NULL || result == NULL || result_size < 3) {
    return -1;
  }

  size_t pos = 0;
  result[pos++] = '"';  // Opening quote

  for (const char *src = value; *src && pos < result_size - 2; src++) {
    if (*src == '"' || *src == '\\') {
      // Escape quotes and backslashes
      if (pos < result_size - 3) {
        result[pos++] = '\\';
        result[pos++] = *src;
      } else {
        return -1;  // Not enough space
      }
    } else {
      result[pos++] = *src;
    }
  }

  if (pos < result_size - 1) {
    result[pos++] = '"';  // Closing quote
    result[pos] = '\0';   // Null terminator
    return (int)pos;
  }

  return -1;  // Not enough space
}

/**
 * @brief Implements token pasting (##) operator.
 *
 * This function finds ## operators in the macro expansion result and concatenates
 * the tokens on either side of them. This is called after parameter substitution
 * to perform the final token pasting step.
 *
 * @param buf Pointer to the start of the buffer.
 * @param endmacro Pointer to the end of the macro in the buffer.
 */
void removeDoubleHash(char *buf, char *endmacro)
{
  char *paste_pos = buf;

  while (paste_pos < endmacro - 1) {
    if (*paste_pos == '#' && *(paste_pos + 1) == '#') {
      // Find the end of the left token (skip backwards over whitespace)
      char *left_end = paste_pos - 1;
      while (left_end >= buf && isspace(*left_end)) {
        left_end--;
      }

      // Find the start of the left token
      char *left_start = left_end;
      if (left_end >= buf && *left_end == '"') {
        // Handle string literal - find the opening quote
        left_start--; // Move past the closing quote
        while (left_start > buf && *left_start != '"') {
          left_start--;
        }
      } else {
        // Handle identifier
        while (left_start > buf && (isalnum(*(left_start - 1)) || *(left_start - 1) == '_')) {
          left_start--;
        }
      }

      // Find the start of the right token (skip forwards over whitespace)
      char *right_start = paste_pos + 2;
      while (right_start < endmacro && isspace(*right_start)) {
        right_start++;
      }

      // Find the end of the right token
      char *right_end = right_start;
      if (right_start < endmacro && *right_start == '"') {
        // Handle string literal - find the closing quote
        right_end = right_start + 1;
        while (right_end < endmacro && *right_end != '"') {
          right_end++;
        }
        if (right_end < endmacro && *right_end == '"') {
          right_end++; // Include the closing quote
        }
      } else if (right_start < endmacro && isdigit(*right_start)) {
        // Handle number literal - include all alphanumeric characters for hex, floats, etc.
        while (right_end < endmacro && (isalnum(*right_end) || *right_end == '.')) {
          right_end++;
        }
      } else {
        // Handle identifier
        while (right_end < endmacro && (isalnum(*right_end) || *right_end == '_')) {
          right_end++;
        }
      }

      if (left_start <= left_end && right_start < right_end) {
        // Calculate sizes
        int left_len = left_end - left_start + 1;
        int right_len = right_end - right_start;
        int paste_section_len = right_end - left_start;  // Total section being replaced

        // Special case: pasting string literal with non-string token
        if (left_start < left_end && *left_start == '"' && *left_end == '"' &&
            !(right_start < right_end && *right_start == '"')) {
          // Merge non-string token into the string literal
          char pasted[256];
          int pos = 0;
          
          // Copy opening quote and string content (excluding closing quote)
          for (char *p = left_start; p < left_end; p++) {
            if (pos < (int)sizeof(pasted) - 1) {
              pasted[pos++] = *p;
            }
          }
          
          // Copy right token content
          for (char *p = right_start; p < right_end; p++) {
            if (pos < (int)sizeof(pasted) - 1) {
              pasted[pos++] = *p;
            }
          }
          
          // Add closing quote
          if (pos < (int)sizeof(pasted) - 1) {
            pasted[pos++] = '"';
          }
          pasted[pos] = '\0';
          
          int new_len = pos;
          
          if (new_len < (int)sizeof(pasted)) {
            // Calculate how much content follows the right token
            char *end_of_buffer = endmacro + strlen(endmacro);
            int remaining_len = end_of_buffer - right_end;

            // Replace the entire section with pasted token
            int size_diff = new_len - paste_section_len;
            if (size_diff != 0) {
              memmove(left_start + new_len, right_end, remaining_len + 1);
            }

            // Copy the pasted token into position
            memcpy(left_start, pasted, new_len);

            // Continue from after the pasted token
            paste_pos = left_start + new_len;
          } else {
            // Token too long, just remove the ##
            memmove(paste_pos, paste_pos + 2, strlen(paste_pos + 2) + 1);
          }
        } else {
          // Regular token pasting
          int new_len = left_len + right_len;  // Length after pasting

          // Create the pasted token
          char pasted[256];  // Temporary buffer for pasted token
          if ((size_t)new_len < sizeof(pasted)) {
            strncpy(pasted, left_start, left_len);
            strncpy(pasted + left_len, right_start, right_len);
            pasted[new_len] = '\0';

            // Calculate how much content follows the right token
            char *end_of_buffer = endmacro + strlen(endmacro);
            int remaining_len = end_of_buffer - right_end;

            // Replace the entire section (left_token ## right_token) with pasted token
            int size_diff = new_len - paste_section_len;
            if (size_diff != 0) {
              memmove(left_start + new_len, right_end, remaining_len + 1);
            }

            // Copy the pasted token into position
            memcpy(left_start, pasted, new_len);

            // Continue from after the pasted token
            paste_pos = left_start + new_len;
          } else {
            // Token too long, just remove the ##
            memmove(paste_pos, paste_pos + 2, strlen(paste_pos + 2) + 1);
          }
        }
      } else {
        // Invalid pasting context, just remove the ##
        memmove(paste_pos, paste_pos + 2, strlen(paste_pos + 2) + 1);
      }
    } else {
      paste_pos++;
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
 * @param ifclausemode Flag indicating if the macro is inside an #if clause.
 * @return 0 if the macro was successfully processed, -1 if an error occurred, or the number of characters processed if no macro was found.
 */
int processMacro(char *buf, int len, int ifclausemode)
{
  char * const start = buf, *end = buf + len;
  Macro *parammacrolist = NULL;

  while (buf <= end && isIdent(*buf, buf - start)) {
    buf++;
  }
  if (buf == start) {  // no valid identifier, therefore advance one char
    return 1;
  }
  Macro *macro = findMacro(start, buf);
  if (macro == NULL) {  // no macro found, check for built-in macros
    char *builtin_value = getBuiltinMacro(start, buf);
    if (builtin_value != NULL) {
      // Found a built-in macro, replace it
      buf = replaceBuf(start, buf, end, builtin_value);
      macro_expanded = 1;  // Mark that a macro was expanded
      return buf - start;
    }

    if (*buf == '(') {  // skip functional macro
      buf = skipExpression(buf, end);
    }
    if (ifclausemode)
      buf = replaceBuf(start, buf, end, "0");
    return buf - start;
  }
  DPRINT("processMacro: found %s\n", macro->name);
  macro_expanded = 1;  // Mark that a macro was expanded
  MacroParam *param = macro->param;
  if (param != NULL) {  // functional macro
    Macro *parammacro = NULL;
    // check for '(', there are no white spaces allowed
    if (*buf != '(') {
      DPRINT("processMacro: missing '(' for macro %s\n", macro->name);
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
        if (param->name == NULL) {  // functional macro without parameter
          if (buf == paramstart) {
            buf++;  // Advance past the closing )
            break;
          } else {
            DPRINT("processMacro: error no parameter expected\n");
            return -1;  // error extra parameter in functional macro not expected
          }
        } else {
          if (param->next != NULL) {  // error not enough parameters
            DPRINT("processMacro: error not enough parameters\n");
            return -1;
          }
        }
      } else if (*buf == ',') {
        if (param->next == NULL) {  // error too many parameters
          DPRINT("processMacro: error too many parameters\n");
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

  if (ifclausemode && (macro->replace == NULL || *macro->replace == '\0')) {
    buf = replaceBuf(start, buf, end, "0");
    return buf - start;  // Return position after replacement
  }

  buf = replaceBuf(start, buf, end, macro->replace);
  if (buf == NULL) {  // buffer too small
    return -1;
  }

  if (parammacrolist != NULL) {
    char *token = start;
    while (token < buf) {
      if (*token == '#' && token + 1 < buf && isIdent(*(token + 1), 0) &&
          !(token > start && *(token - 1) == '#')) {
        // Check for stringification: # followed by parameter name
        // But skip if this # is part of ## (preceded by another #)
        char *param_start = token + 1;
        char *param_end = param_start;
        while (param_end < buf && isIdent(*param_end, param_end - param_start)) {
          param_end++;
        }

        // Check if this is a valid parameter name
        Macro *parammacro = parammacrolist;
        while (parammacro != NULL) {
          if (strlen(parammacro->name) == (size_t)(param_end - param_start) &&
              strncmp(parammacro->name, param_start, param_end - param_start) == 0) {

            // Stringify the parameter
            char stringified[512];
            char expanded_param[512];
            
            // First expand the parameter value
            strncpy(expanded_param, parammacro->replace, sizeof(expanded_param) - 1);
            expanded_param[sizeof(expanded_param) - 1] = '\0';
            
            // Process the parameter value to expand any macros in it
            int expanded_len = strlen(expanded_param);
            if (processBuffer(expanded_param, expanded_len, 0) < 0) {
              // If expansion fails, use original value
              strncpy(expanded_param, parammacro->replace, sizeof(expanded_param) - 1);
              expanded_param[sizeof(expanded_param) - 1] = '\0';
            }
            
            int str_len = stringifyParameter(expanded_param, stringified, sizeof(stringified));

            if (str_len > 0) {
              char *newtoken = replaceBuf(token, param_end, end, stringified);
              if (newtoken == NULL) {  // buffer too small
                return -1;
              }
              buf += newtoken - param_end;
              token = newtoken;
            } else {
              token = param_end;
            }
            break;
          }
          parammacro = parammacro->next;
        }

        if (parammacro == NULL) {
          // Not a parameter, move past the #
          token++;
        }
      } else if (isIdent(*token, token - start)) {
        // Regular parameter substitution
        char *tokenend = token;
        while (tokenend < buf && isIdent(*tokenend, tokenend - token)) {
          tokenend++;
        }
        Macro *parammacro = parammacrolist;
        while (parammacro != NULL) {
          if (strlen(parammacro->name) == (size_t)(tokenend - token)
          && strncmp(parammacro->name, token, strlen(parammacro->name)) == 0) {
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
        if (parammacro == NULL) {
          token++;
        }
      } else {
        token++;
      }
    }

    // free memory of parammacro's
    for (Macro *parammacro = parammacrolist, *next; parammacro != NULL; parammacro = next) {
      next = parammacro->next;
      free(parammacro->replace);
      free(parammacro);
    }
  }

  removeDoubleHash(start, buf);
  DPRINT("processMacro done: %s\n", start);
  return buf - start;  // Return new offset from start
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
int processBuffer(char *buf, int len, int ifclausemode)
{
  // Scan buf to recognize macros
  char *start = buf, *end = buf + len;
  int restart_count = 0;
  const int MAX_RESTARTS = 100;  // Prevent infinite loops

  while (buf < end && *buf != '\0') {
    buf = skipSpaces(buf, end);  // skip preceding spaces
    if (isIdent(*buf, 0)) {
      DPRINT("processBuffer next: %.*s\n", (int)(end - buf), buf);
      char *macro_start = buf;
      macro_expanded = 0;  // Reset flag
      int cnt = processMacro(buf, end - buf, ifclausemode);
      DPRINT("processBuffer next done: %s\n", buf);
      if (cnt < 0) {
        DPRINT("processBuffer: failed %d\n", cnt);
        return cnt;
      }

      if (macro_expanded) {  // A macro was expanded
        // Restart processing from the beginning of the replacement to handle recursion
        if (restart_count < MAX_RESTARTS) {
          restart_count++;
          buf = macro_start;  // Restart from where the macro was
          continue;
        } else {
          DPRINT("processBuffer: max restarts reached, continuing\n");
          buf += cnt;
          restart_count = 0;
        }
      } else {
        buf += cnt;
        restart_count = 0;  // Reset restart count
      }
      continue;
    }
    // skip string
    if (*buf == '\"' && !(buf > start && *(buf - 1) == '\\')) {
      buf = skipString(buf, end);
      continue;
    }
    // ignore char
    if (isdigit(*buf)) {
      buf = skipNumber(buf, end);
      continue;
    }
    buf++;
    restart_count = 0;  // Reset restart count when advancing normally
  }
  DPRINT("processBuffer done: %s\n", start);
  return 0;
}


/**
 * @brief Get replacement text for built-in macros.
 *
 * This function handles built-in preprocessor macros like __LINE__ and __FILE__.
 * It returns dynamically generated replacement text for these macros.
 *
 * @param start The start of the macro name.
 * @param end The end of the macro name.
 * @return A dynamically allocated string containing the replacement text, or NULL if not a built-in macro.
 */
char *getBuiltinMacro(char *start, char *end) {
    static char line_buffer[32];
    static char file_buffer[512];
    int name_len = end - start;

    // Check for __LINE__ macro
    if (name_len == 8 && strncmp(start, "__LINE__", 8) == 0) {
        // Get current input stream to get line number
        instream_t *current = getcurrentinstream();
        if (current != NULL) {
            // Subtract 1 because line number has been incremented after processing the newline
            snprintf(line_buffer, sizeof(line_buffer), "%d", current->line - 1);
            return line_buffer;
        } else {
            return "1";  // Default line number if no stream
        }
    }

    // Check for __FILE__ macro
    if (name_len == 8 && strncmp(start, "__FILE__", 8) == 0) {
        // Get current input stream to get filename
        instream_t *current = getcurrentinstream();
        if (current != NULL && current->fname != NULL) {
            snprintf(file_buffer, sizeof(file_buffer), "\"%s\"", current->fname);
            return file_buffer;
        } else {
            return "\"<unknown>\"";  // Default filename if no stream
        }
    }

    // Not a built-in macro
    return NULL;
}
