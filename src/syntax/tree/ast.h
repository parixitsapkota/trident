#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "type/type.h"

#include "core/mem.h"
#include "core/source.h"
#include "syntax/token/token.h"

typedef struct AstNode AstNode;

typedef enum { STATIC_VAR, FRAME_VAR, PARAM_VAR } VarKind;

typedef struct {
    VarKind kind;
    Type *type;
} VarInfo;

typedef enum {
    /* expressions */
    AST_IDENT,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_CHAR,
    AST_UNARY,
    AST_BINARY,
    AST_ASSIGN,
    AST_INDEX,
    AST_CALL,

    /* statements */
    AST_BLOCK,
    AST_EXPR_STMT,
    AST_IF,
    AST_ELSE_IF,
    AST_ELSE,
    AST_LOOP,
    AST_WHILE,
    AST_MATCH,
    AST_CASE,
    AST_BREAK,
    AST_CONTINUE,
    AST_RETURN,

    /* declarations */
    AST_FUNCTION,
} AstKind;

struct AstNode {
    AstKind kind;
    Span *span;
    AstNode *next;
    Type *type;

    union {
        /* AST_IDENT, AST_INT, AST_FLOAT, AST_STRING, AST_CHAR, */
        struct {
            const char *name;
            VarInfo *info;
        } ident;
        size_t int_val;
        float float_val;
        const char *str;
        char char_val;

        /* AST_UNARY */
        struct {
            TokenKind op;
            bool postfix;
            AstNode *operand;
        } unary;

        /* AST_BINARY, AST_ASSIGN */
        struct {
            TokenKind op;
            AstNode *lhs;
            AstNode *rhs;
        } binary;

        /* AST_IF, AST_ELSE_IF */
        struct {
            AstNode *cond;
            AstNode *then_b;
            AstNode *else_b;
        } cond;

        /* AST_WHILE, AST_MATCH, AST_CASE */
        struct {
            AstNode *expr;
            AstNode *body;
        } ctl;

        /* AST_INDEX */
        struct {
            AstNode *left;
            AstNode *index;
        } index;

        /* AST_CALL: args is a linked list */
        struct {
            const char *callee;
            AstNode *args;
            size_t argc;
        } call;

        /* AST_BLOCK: stmts is a linked list */
        struct {
            AstNode *stmts;
        } block;

        /* AST_EXPR_STMT, AST_RETURN, AST_ELSE, AST_LOOP */
        struct {
            AstNode *expr;
        } stmt;

        /* AST_FUNCTION */
        struct {
            const char *name;
            AstNode *params;
            size_t nparams;
            AstNode *body;
        } func;
    } as;
};

VarInfo *var_info(Arena *arena, VarKind kind, Type *type);

AstNode *ast_new(Arena *arena, AstKind kind, Span *span);

AstNode *ast_ident_val(Arena *a, Span *span, const char *name);
AstNode *ast_int_val(Arena *a, Span *span, size_t int_val);
AstNode *ast_float_val(Arena *a, Span *span, float float_val);
AstNode *ast_char_val(Arena *a, Span *span, char char_val);
AstNode *ast_string_val(Arena *a, Span *span, const char *str);

AstNode *ast_unary(Arena *a, Span *span, TokenKind op, bool postfix, AstNode *operand);
AstNode *ast_binary(Arena *a, Span *span, AstKind kind, TokenKind op, AstNode *left,
                    AstNode *right);
AstNode *ast_index(Arena *a, Span *span, AstNode *left, AstNode *index);
AstNode *ast_call(Arena *a, Span *span, const char *callee, AstNode *args, size_t argc);

AstNode *ast_stmt(Arena *a, Span *span, AstKind kind, AstNode *expr);
AstNode *ast_cond(Arena *a, Span *span, AstNode *cond, AstNode *then_b, AstNode *else_b);
AstNode *ast_ctl(Arena *a, Span *span, AstKind kind, AstNode *cond, AstNode *body);
AstNode *ast_block(Arena *a, Span *span, AstNode *stmts);

AstNode *ast_function(Arena *a, Span *span, const char *name, AstNode *params,
                      size_t nparams, AstNode *body);

void print_ast(AstNode *root);
