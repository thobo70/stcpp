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

#ifndef MACRO_H
#define MACRO_H

// Function prototypes
int addMacro(char *buf);
int deleteMacro(char *buf);
int processBuffer(char *buf, int len);
void printMacroList();
int isdefinedMacro(char *start, char *end);
int isIdent(char c, int idx);
char *replaceBuf(char *start, char *buf, char *end, char *replace);

#endif  // MACRO_H
