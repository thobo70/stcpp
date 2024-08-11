/**
 * @file expr.h
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef EXPR_H
#define EXPR_H


#include <stdlib.h>


typedef long eval_t;

typedef enum optoken {
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_ADD,
  OP_SUB,
  OP_LSHIFT,
  OP_RSHIFT,
  OP_LT,
  OP_LE,
  OP_GT,
  OP_GE,
  OP_EQ,
  OP_NE,
  OP_AND,
  OP_XOR,
  OP_OR,
  OP_LAND,
  OP_LOR,
  OP_COND,
  OP_CONDBODY,
  OP_NUM,
  OP_ERR
} optoken_t;

typedef struct opinfo {
  const char *name;
  optoken_t token;
  int precedence;
} opinfo_t;

typedef struct astnode {
  struct astnode *parent;
  struct astnode *left;
  struct astnode *right;
  const opinfo_t *opinfo;
  eval_t val;
} astnode_t;



astnode_t *evalexpr(char **buf);
void freenode(astnode_t *node);
astnode_t *evalnode(astnode_t *node, int cond);
astnode_t *getroot(astnode_t *node);


#endif
