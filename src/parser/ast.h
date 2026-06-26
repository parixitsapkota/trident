#ifndef TRIDENT_AST_H
#define TRIDENT_AST_H

#include <stdbool.h>
#include <stddef.h>

#include "lexer/lexer.h"
#include "shi_opa.h"

typedef struct AstNode AstNode;

typedef struct AstCompound {
  AstNode *node;
  struct AstCompound *next;
} AstCompound;

typedef struct {
  const char *value;
  TokenKind kind;
} AstAtom;

typedef struct {
  const char *name;
  AstCompound *args;
} AstFuncCall;

typedef struct {
  AstNode *node;
  TokenKind op;
} AstUnary;

typedef struct {
  AstNode *left;
  TokenKind op;
  AstNode *right;
} AstBinary;

typedef struct {
  bool constant;
  const char *name;
  AstNode *expr;
} AstVarDecl;

typedef struct {
  bool constant;
  bool define;
  const char *name;
  AstCompound *body;
} AstStion;

typedef struct {
  AstNode *condition;
  AstCompound *body;
} AstWhile;

typedef struct AstIfStmt {
  AstNode *condition;
  AstCompound *body;
  AstNode *next;
} AstIfStmt;

typedef struct {
  bool pointer;
  const char *name;
  AstCompound *args;
  const char *return_type;
  AstCompound *body;
} AstFuncDecl;

typedef enum {
  AST_ATOM,
  AST_FUNCTION_CALL,
  AST_UNARY,
  AST_BINARY,
  AST_EXPRESSION,
  AST_COMPOUND,
  AST_FUNCTION_DECL,
  AST_VAR_DECL,
  AST_STRUCT,
  AST_UNION,
  AST_LOOP,
  AST_WHILE,
  AST_IF_STMT,
  AST_ELSE_IF_STMT,
  AST_ELSE_STMT,
  AST_RETURN,
  AST_BREAK,
  AST_CONTINUE,
} AstKind;

struct AstNode {
  AstKind kind;
  union {
    AstCompound compound;
    AstAtom atom;
    AstFuncCall func_call;
    AstUnary unary;
    AstBinary binary;
    AstVarDecl var_decl;
    AstStion ast_station;
    AstWhile while_;
    AstIfStmt if_stmt;
    AstFuncDecl func_decl;
  };
  size_t line;
  size_t col;
};

#endif // TRIDENT_AST_H
