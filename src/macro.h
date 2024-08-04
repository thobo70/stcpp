/**
 * @file macro.h
 * @brief Header file for macro processing functions.
 * @author Thomas Boos (tboos70@gmail.com)
 * @version 0.1
 * @date 2024-03-30
 * 
 * This file contains the function prototypes for the macro processing functions
 * implemented in macro.c. These functions include addMacro, deleteMacro,
 * processBuffer, printMacroList, and isdefinedMacro.
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

#endif // MACRO_H