/**
 * @file macro.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief Header file for macro processing functionality.
 * @version 0.1
 * @date 2024-08-09
 *
 * @copyright Copyright (c) 2024
 *
 * This header file contains function declarations for macro processing,
 * including macro definition, deletion, expansion, and built-in macro support.
 */

#ifndef SRC_MACRO_H_
#define SRC_MACRO_H_

/**
 * @brief Adds a macro definition to the macro list.
 * 
 * Parses a macro definition string, extracts the macro name, parameters (if any),
 * and replacement text, then adds it to the global macro list. Supports both
 * simple macros and function-like macros with parameters.
 * 
 * @param buf Buffer containing the macro definition string.
 * @return 0 on success, -1 on error (invalid macro syntax).
 */
int addMacro(char *buf);

/**
 * @brief Adds a macro definition from command line -D option.
 * 
 * Parses command line macro definitions in formats:
 * - MACRO (defines MACRO as 1)
 * - MACRO=value (defines MACRO as value)
 * - MACRO= (defines MACRO as empty string)
 * 
 * @param definition The macro definition string from -D option.
 * @return 0 on success, -1 on error (invalid definition format).
 */
int addCommandLineMacro(const char *definition);

/**
 * @brief Deletes a macro from the macro list.
 * 
 * Searches for a macro with the specified name and removes it from the macro list,
 * freeing all associated memory including parameters and replacement text.
 * 
 * @param buf Name of the macro to delete.
 * @return 0 if macro was deleted, -1 if macro was not found.
 */
int deleteMacro(char *buf);

/**
 * @brief Bans a macro name from being defined.
 * 
 * Adds a macro name to the banned list, preventing it from being defined later.
 * If the macro is already defined, it is first deleted, then added to the banned list.
 * This implements the -U command line option functionality.
 * 
 * @param name Name of the macro to ban.
 * @return 0 on success, -1 on error (memory allocation failure).
 */
int banMacro(char *name);

/**
 * @brief Checks if a macro name is banned.
 * 
 * Searches the banned macro list to determine if a macro name has been
 * banned using the -U command line option.
 * 
 * @param name Name of the macro to check.
 * @return 1 if macro is banned, 0 if not banned.
 */
int isMacroBanned(char *name);

/**
 * @brief Processes a buffer to recognize and replace macros.
 * 
 * Scans the buffer for macro identifiers and replaces them with their
 * replacement text. Handles both simple macros and function-like macros
 * with parameter substitution, stringification (#), and token pasting (##).
 * 
 * @param buf Buffer containing text to process.
 * @param len Length of the buffer.
 * @param ifclausemode 1 if processing within #if clause (undefined macros become 0), 0 otherwise.
 * @return 0 on success, -1 on error (buffer too small, syntax error).
 */
int processBuffer(char *buf, int len, int ifclausemode);

/**
 * @brief Prints the current list of defined macros.
 * 
 * Iterates through the macro list and prints each macro's name, parameters
 * (if any), and replacement text. Used for debugging purposes.
 */
void printMacroList();

/**
 * @brief Checks if a macro is defined.
 * 
 * Searches the macro list for a macro with the specified name range.
 * Used by #ifdef, #ifndef, and defined() operator.
 * 
 * @param start Pointer to the start of the macro name.
 * @param end Pointer to the end of the macro name.
 * @return 1 if macro is defined, 0 if not defined.
 */
int isdefinedMacro(char *start, char *end);

/**
 * @brief Checks if a character is a valid identifier character.
 * 
 * Determines whether a character can be part of a C identifier.
 * First character must be letter or underscore, subsequent characters
 * can be letters, digits, or underscores.
 * 
 * @param c The character to check.
 * @param idx Position of the character in the identifier (0 for first character).
 * @return 1 if character is valid for the position, 0 otherwise.
 */
int isIdent(char c, int idx);

/**
 * @brief Replaces a portion of a buffer with a replacement string.
 * 
 * Replaces the text from 'buf' to the end of the string with 'replace',
 * shifting the remaining buffer content as needed. Used during macro expansion.
 * 
 * @param start Pointer to the start of the buffer.
 * @param buf Pointer to the portion to be replaced.
 * @param end Pointer to the end of the buffer.
 * @param replace Replacement string to insert.
 * @return Pointer to position after replacement, or NULL if buffer too small.
 */
char *replaceBuf(char *start, char *buf, char *end, char *replace);

/**
 * @brief Gets replacement text for built-in macros.
 * 
 * Handles built-in preprocessor macros like __LINE__ and __FILE__.
 * Returns dynamically generated replacement text based on current
 * input stream context.
 * 
 * @param start Pointer to the start of the macro name.
 * @param end Pointer to the end of the macro name.
 * @return Pointer to replacement text, or NULL if not a built-in macro.
 */
char *getBuiltinMacro(char *start, char *end);

#endif  // SRC_MACRO_H_
