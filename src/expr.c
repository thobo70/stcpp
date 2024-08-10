/**
 * @file expr.c
 * @author Thomas Boos (tboos70@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"  // NOLINT
#include "expr.h"  // NOLINT
#include "macro.h"  // NOLINT


#define SET_OP_NUM (&opinfo[sizeof(opinfo) / sizeof(opinfo_t) - 1])   // last should be OP_NUM

opinfo_t opinfo[] = {
  {"<<", OP_LSHIFT, 5},
  {">>", OP_RSHIFT, 5},
  {"<=", OP_LE, 6},
  {">=", OP_GE, 6},
  {"==", OP_EQ, 7},
  {"!=", OP_NE, 7},
  {"&&", OP_LAND, 11},
  {"||", OP_LOR, 12},
  {"<", OP_LT, 6},
  {">", OP_GT, 6},
  {"&", OP_AND, 8},
  {"^", OP_XOR, 9},
  {"|", OP_OR, 10},
  {"*", OP_MUL, 3},
  {"/", OP_DIV, 3},
  {"%", OP_MOD, 3},
  {"+", OP_ADD, 4},
  {"-", OP_SUB, 4},
  {"?", OP_COND, 13},
  {":", OP_CONDBODY, 13},
  {NULL, OP_NUM, 0}
};



void freenode(astnode_t *node)
{
  if (node == NULL) {
    return;
  }
  if (node->left != NULL) {
    freenode(node->left);
  }
  if (node->right != NULL) {
    freenode(node->right);
  }
  free(node);
}



astnode_t *getroot(astnode_t *node)
{
  if (node == NULL) {
    return NULL;
  }
  while (node->parent != NULL) {
    node = node->parent;
  }
  return node;
}



astnode_t *evalnode(astnode_t *node, int cond)
{
  if (node == NULL || node->opinfo == NULL) {
    freenode(node);
    return NULL;
  }
  if (node->opinfo->token == OP_NUM) {
    return node;
  }
  if (node->opinfo->token != OP_CONDBODY || (node->opinfo->token == OP_CONDBODY && cond)) {
    node->left = evalnode(node->left, cond);
    if (node->left == NULL) {
      freenode(node);
      return NULL;
    }
  }
  if (node->opinfo->token == OP_COND) {
    cond = node->left->val;
  }
  if (node->opinfo->token != OP_CONDBODY || (node->opinfo->token == OP_CONDBODY && !cond)) {
    node->right = evalnode(node->right, cond);
    if (node->right == NULL) {
      freenode(node);
      return NULL;
    }
  }
  switch (node->opinfo->token) {
    case OP_MUL:
      node->val = node->left->val * node->right->val;
      break;
    case OP_DIV:
      if (node->right->val == 0) {
        freenode(node);
        return NULL;
      }
      node->val = node->left->val / node->right->val;
      break;
    case OP_MOD:
      if (node->right->val == 0) {
        freenode(node);
        return NULL;
      }
      node->val = node->left->val % node->right->val;
      break;
    case OP_ADD:
      node->val = node->left->val + node->right->val;
      break;
    case OP_SUB:
      node->val = node->left->val - node->right->val;
      break;
    case OP_LSHIFT:
      node->val = node->left->val << node->right->val;
      break;
    case OP_RSHIFT:
      node->val = node->left->val >> node->right->val;
      break;
    case OP_LT:
      node->val = node->left->val < node->right->val;
      break;
    case OP_LE:
      node->val = node->left->val <= node->right->val;
      break;
    case OP_GT:
      node->val = node->left->val > node->right->val;
      break;
    case OP_GE:
      node->val = node->left->val >= node->right->val;
      break;
    case OP_EQ:
      node->val = node->left->val == node->right->val;
      break;
    case OP_NE:
      node->val = node->left->val != node->right->val;
      break;
    case OP_AND:
      node->val = node->left->val & node->right->val;
      break;
    case OP_XOR:
      node->val = node->left->val ^ node->right->val;
      break;
    case OP_OR:
      node->val = node->left->val | node->right->val;
      break;
    case OP_LAND:
      node->val = node->left->val && node->right->val;
      break;
    case OP_LOR:
      node->val = node->left->val || node->right->val;
      break;
    case OP_COND:
      node->val = node->right->val;
      break;
    case OP_CONDBODY:
      node->val = cond ? node->left->val : node->right->val;
      break;
    default:
      // cppcheck-suppress syntaxError
      DPRINT("evalnode: invalid operator %d\n", node->opinfo->token);
      freenode(node);
      return NULL;
  }
  if (node->left != NULL) {
    freenode(node->left);
  }
  if (node->right != NULL) {
    freenode(node->right);
  }
  DPRINT("eval: %s %d -> %d\n", node->opinfo->name, cond, node->val);
  node->opinfo = SET_OP_NUM;   // last should be OP_NUM
  node->left = NULL;
  node->right = NULL;
  return node;
}



astnode_t *newnode(void)
{
  astnode_t *node = malloc(sizeof(astnode_t));
  if (node == NULL) {
    return NULL;
  }
  memset(node, 0, sizeof(astnode_t));
  return node;
}



astnode_t *evaloperand(char **buf)
{
  astnode_t *node = NULL;

  DPRINT("evaloperand: %s\n", *buf);

  char c = **buf;
  (*buf)++;
  if (strchr("(+-!~", c) != NULL)
  {
    if (c == '(')
      node = evalexpr(buf);
    else
      node = evaloperand(buf);
    if (node == NULL)
      return NULL;
    switch (c)
    {
    case '(':
      if (**buf != ')')
      {
        fprintf(stderr, "Syntax error: missing ')'\n");
        return NULL;
      }
      (*buf)++;
      evalnode(node, 1);
      if (node != NULL && node->opinfo->token == OP_NUM)
        DPRINT("evalexpr: %d\n", node->val);
      return node;
    case '!':
      node->val = !node->val;
      return node;
    case '~':
      node->val = ~node->val;
      return node;
    case '+':
      return node;
    case '-':
      node->val = -node->val;
      return node;
    }
    return NULL;      // should never reach here
  }
  node = newnode();
  if (node == NULL)
    return NULL;
  node->opinfo = SET_OP_NUM;
  if (isdigit(c))
  {
    int base = 10;
    if (c == '0')
    {
      switch (**buf) {
      case 'x':
        base = 16;
        (*buf)++;
        break;
      case 'b':
        base = 2;
        (*buf)++;
        break;
      default:
        base = 8;
        break;
      }
    } else {
      node->val = c - '0';
    }
    while (isdigit(**buf) || (base == 16 && isxdigit(**buf))) {
      eval_t digit = 0;
      node->val *= base;
      if (isdigit(**buf)) {
        digit = (**buf - '0');
      } else {
        digit = (tolower(**buf) - 'a' + 10);
      }
      if (digit >= base) {
        fprintf(stderr, "Syntax error: invalid digit\n");
        freenode(node);
        return NULL;
      }
      (*buf)++;
      node->val += digit;
    }
    if (**buf != 0 && strchr("uUlL", **buf) != NULL) {
      (*buf)++;
    }
    return node;
  }
  if (isIdent(c, 0)) {
    char *start = *buf - 1;
    while (isIdent(**buf, *buf - start)) {
      (*buf)++;
    }
    node->val = isdefinedMacro(start, *buf);
    return node;
  }
  if (c == '\'') {
    if (**buf == '\\') {
      (*buf)++;
      switch (**buf) {
        case 'n':
          node->val = '\n';
          break;
        case 't':
          node->val = '\t';
          break;
        case 'v':
          node->val = '\v';
          break;
        case 'b':
          node->val = '\b';
          break;
        case 'r':
          node->val = '\r';
          break;
        case 'f':
          node->val = '\f';
          break;
        case 'a':
          node->val = '\a';
          break;
        case '\\':
          node->val = '\\';
          break;
        case '\'':
          node->val = '\'';
          break;
        case '\"':
          node->val = '\"';
          break;
        case '\?':
          node->val = '\?';
          break;
        case '0':
          node->val = '\0';
          break;
        default:
          fprintf(stderr, "Syntax error: invalid escape sequence\n");
          freenode(node);
          return NULL;
      }
    } else {
      node->val = **buf;
    }
    (*buf)++;
    if (**buf != '\'') {
      fprintf(stderr, "Syntax error: missing closing quote\n");
      freenode(node);
      return NULL;
    }
    (*buf)++;
    return node;
  }

  // no operand found, we assume empty Macros will be 0
  // @todo: check if this is correct
  node->val = 0;
  (*buf)--;  // rewind
  return node;
  /*
  freenode(node);
  fprintf(stderr, "Syntax error: invalid operand >%s<\n", *buf - 1);
  return NULL;
  */
}



/**
 * Integer constants.
 * Character constants, which are interpreted as they would be in normal code.
 * Arithmetic operators for addition, subtraction, multiplication, division,
 * bitwise operations, shifts, comparisons, and logical operations (&& and ||).
 * The latter two obey the usual short-circuiting rules of standard C.
 * Macros. All macros in the expression are expanded before actual computation
 * of the expression’s value begins.
 * Uses of the defined operator, which lets you check whether macros are defined
 * in the middle of an ‘#if’.
 * Identifiers that are not macros, which are all considered to be the number
 * zero. This allows you to write #if MACRO instead of #ifdef MACRO, if you
 * know that MACRO, when defined, will always have a nonzero value. Function-like
 * macros used without their function call parentheses are also treated as zero.
 * 
 * 1. ()
 * 2. ! ~ + -     <--- unary operators not needed, evaluates already with evaloperand
 * 3. * / %
 * 4. + -
 * 5. << >>
 * 6. < <= > >=
 * 7. == !=
 * 8. &
 * 9. ^
 * 10. |
 * 11. &&
 * 12. ||
 * 13. ?:
 */

const opinfo_t *getopinfo(char **buf)
{
  DPRINT("getopinfopre: %s\n", *buf);
  for (int i = 0; i < (int)(sizeof(opinfo) / sizeof(opinfo[0])); i++) {  // NOLINT
    if (opinfo[i].name == NULL) {
      continue;
    }
    if (strncmp(*buf, opinfo[i].name, strlen(opinfo[i].name)) == 0) {
      *buf += strlen(opinfo[i].name);
      DPRINT("getopinfopost: %s\n", *buf);
      return &opinfo[i];
    }
  }
  return NULL;
}



astnode_t *evalexpr(char **buf)
{
  assert(buf != NULL && *buf != NULL);
  astnode_t *node, *t;

  DPRINT("evalexpr: %s\n", *buf);

  node = evaloperand(buf);
  if (node != NULL)  DPRINT("operand: %d\n", node->val);
  if (node == NULL || **buf == '\0' || **buf == ')') {
    return node;
  }
  t = newnode();
  if (t == NULL) {
    freenode(node);
    return NULL;
  }
  t->left = node;
  node = t;
  node->left->parent = node;
  node->opinfo = getopinfo(buf);
  if (node->opinfo == NULL) {
    fprintf(stderr, "Syntax error: invalid operator\n");
    freenode(node);
    return NULL;
  }
  t = evalexpr(buf);
  if (t == NULL) {
    freenode(node);
    return NULL;
  }
  if (t->opinfo->token == OP_NUM) {
    node->right = t;
    t->parent = node;
    return node;
  }
  if (t->opinfo->precedence > node->opinfo->precedence) {
    node->right = t->left;
    t->left->parent = node;
    while (node->parent != NULL && t->opinfo->precedence > node->parent->opinfo->precedence) {
      node = node->parent;
    }
    t->parent = node->parent;
    t->left = node;
    if (node->parent != NULL) {
      if (node->parent->left == node) {
        node->parent->left = t;
      } else {
        node->parent->right = t;
      }
    }
    node->parent = t;
    return t;
  }
  node->right = t;
  t->parent = node;

  return node;
}



