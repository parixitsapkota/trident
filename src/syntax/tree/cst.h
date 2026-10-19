#pragma once

#include "core/arena.h"
#include "core/span.h"
#include "syntax/token/token.h"

typedef enum {
    CST_ROOT,
    CST_UNARY,
    CST_BINARY,
    CST_LEAF,
} CstKind;

typedef struct CstNode CstNode;
struct CstNode {
    CstKind kind;
    Span span;
    Token *trivia_predede;
    Token *trivia_follow;
    CstNode *next;

    union {
        // LITERALS
        struct {
            Token *tok;
        } leaf;

        // UNARY EXPRESSION
        struct {
            CstNode *operand;
            Token *op;
        } unary;

        // BINARY EXPRESSION
        struct {
            CstNode *lhs;
            Token *op;
            CstNode *rhs;
        } binary;
    } as;
};

CstNode *new_cst_node(Arena *arena, CstKind kind, Span span);

CstNode *new_cst_leaf(Arena *arena, Token *token, Span span);
CstNode *new_cst_unary(Arena *arena, CstNode *operand, Token *op, Span span);
CstNode *new_cst_binary(Arena *arena, CstNode *lhs, Token *op, CstNode *rhs, Span span);
