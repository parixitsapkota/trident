#pragma once

#include "parser.h"

#include "core/source.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

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

typedef struct {
    AstNode *first;
    AstNode *last;
} NodeChain;

void chain_append(NodeChain *c, AstNode *head);

Token *current(Parser *p);

Token *next(Parser *p);

Token *pconsume(Parser *p);

bool expect(Parser *p, TokenKind kind);

bool expect_and_consume(Parser *p, TokenKind kind);

bool is_kind(Parser *p, TokenKind kind);

void *parser_error(Parser *p, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
