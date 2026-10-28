#pragma once

#include "parser.h"

#include "core/source/source.h"
#include "syntax/token/token.h"
#include "syntax/ast/ast.h"

struct Parser {
    const Source *src;
    // Token
    Token *tokens;
    Token *tok_tail;
    // AstNodes store
    Arena *nodes;
    // AstNode
    AstNode *root;
    AstNode *tail;
    // error
    size_t errorc;
};

typedef enum {
    PREC_UNKNOWN = -1,
    PREC_NONE = 0,
    PREC_ASSIGNMENT,
    PREC_CONDITIONAL,
    PREC_BIT_OR,
    PREC_BIT_AND,
    PREC_RELATIONAL,
    PREC_EQUALITY,
    PREC_BITSHIFT,
    PREC_ADDITIVE,
    PREC_MULTIPLICATIVE,
} Precedence;

typedef struct {
    AstNode *first;
    AstNode *last;
} NodeChain;

Token *current(Parser *p);
Token *next(Parser *p);
Token *pconsume(Parser *p);
bool expect(Parser *p, TokenKind kind);
bool expect_and_consume(Parser *p, TokenKind kind);
bool is_kind(Parser *p, TokenKind kind);

void chain_append(NodeChain *c, AstNode *head);

void *parser_error(Parser *p, Span *span, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

AstNode *parse_expr(Parser *p, Precedence prec);

AstNode *parse_let_stmt(Parser *p);
AstNode *parse_func_stmt(Parser *p, bool public);
AstNode *parse_stmt(Parser *p);
