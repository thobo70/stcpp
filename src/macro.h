/**
 * @file macro.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SRC_MACRO_H_
#define SRC_MACRO_H_

// Function prototypes
int addMacro(char *buf);
int addCommandLineMacro(const char *definition);
int deleteMacro(char *buf);
int processBuffer(char *buf, int len, int ifclausemode);
void printMacroList();
int isdefinedMacro(char *start, char *end);
int isIdent(char c, int idx);
char *replaceBuf(char *start, char *buf, char *end, char *replace);
char *getBuiltinMacro(char *start, char *end);

#endif  // SRC_MACRO_H_
