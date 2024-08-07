/**
 * @file cmdline.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _CMDLINE_H
#define _CMDLINE_H

typedef enum cmdcondstate {
  COND_IF,
  COND_ELSE
} cmdcondstate_t;


extern int condstate;

int iscmdline(char *line);
int processcmdline(char *buf, int size);


#endif
