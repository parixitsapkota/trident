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
};

typedef struct {
  SHI_OPA *token_pool;
  SHI_OPA *ast;
  size_t index;
} Parser;

SHI_OPA *parser(const SHI_OPA *token_pool);
Token *parser_current(Parser *self);
TokenKind parser_current_kind(Parser *self);
Token *parser_peak(Parser *self);
TokenKind parser_peak_kind(Parser *self);
void parser_consume(Parser *self);
bool parser_Expect(TokenKind kind);

void parse_atom(Parser *p);
void parse_func_call(Parser *p);
void parse_expr(Parser *p);

void parse_var_decl(Parser *p);
void parse_stion(Parser *p);
void parse_while(Parser *p);
void parse_if_stmt(Parser *p);
void parse_body(Parser *p);
void parse_func_decl(Parser *p);

#endif // TRIDENT_PARSER_H
