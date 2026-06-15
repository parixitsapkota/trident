#ifndef TRIDENT_LEXER_H
#define TRIDENT_LEXER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../SHI/shi_opa.h"
#include "../lexer/lexer.h"

typedef struct AstNode AstNode;

typedef struct {
  AstNode **nodes;
  size_t count;
} AstCompound;

typedef struct {
  char *value;
  TokenKind kind;
} AstAtom;

typedef struct {
  char *name;
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
  char *name;
  AstNode *expr;
} AstVarDecl;

typedef struct {
  bool constant;
  bool define;
  char *name;
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
  char *name;
  AstCompound *args;
  char *return_type;
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
  void *node;
};

SHI_OPA *parser(const SHI_OPA *token_pool);

void parse_atom(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_func_call(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_expr(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);

void parse_var_decl(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_stion(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_while(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_if_stmt(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_body(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);
void parse_func_decl(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast);

#endif // TRIDENT_PARSER_H
