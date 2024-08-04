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
