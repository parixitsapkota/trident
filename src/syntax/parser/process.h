#pragma once

#include <stdbool.h>

#include "parser.h"

#include "core/source/source.h"
#include "syntax/ast/ast.h"
#include "syntax/token/token.h"

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
    bool expr_err;
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
Token *expect_and_consume(Parser *p, TokenKind kind);
bool is_kind(Parser *p, TokenKind kind);

bool is_expression_delimiter(TokenKind kind);

void chain_append(NodeChain *c, AstNode *head);
AstNode *base(Parser *p, Token *first);

void *parser_error(Parser *p, Span *span, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

AstNode *parse_any(Parser *p);

AstNode *parse_expr(Parser *p, Precedence prec);
AstNode *parse_paren_expr(Parser *p);

bool parse_constness(Parser *p);
AstNode *parse_type(Parser *p);

AstNode *parse_expr_stmt(Parser *p, AstKind kind);

AstNode *parse_decl_stmt(Parser *p, bool mutable);
AstNode *parse_func_stmt(Parser *p, bool public);

AstNode *parse_block_stmt(Parser *p);

AstNode *parse_structure_stmt(Parser *p, AstKind kind);

AstNode *parse_type_stmt(Parser *p);

AstNode *parse_if_stmt(Parser *p);
AstNode *parse_while_stmt(Parser *p);
AstNode *parse_loop_stmt(Parser *p);
