#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "core/mem/mem.h"
#include "core/source/source.h"
#include "syntax/token/token.h"
#include "type/type.h"

typedef struct AstNode AstNode;

typedef enum {
    AST_PROGRAM,

    /* expressions */
    AST_IDENT,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_CHAR,
    AST_BOOL,
    AST_UNARY,
    AST_BINARY,
    AST_INDEX,
    AST_MEMBER,
    AST_DREF_MEMBER,
    AST_CALL,

    /* statements */
    AST_BLOCK,
    AST_EXPR_STMT,
    AST_IF,
    AST_ELSE_IF,
    AST_ELSE,
    AST_LOOP,
    AST_WHILE,
    AST_LOOP_WHILE,
    AST_MATCH,
    AST_CASE,
    AST_BREAK,
    AST_CONTINUE,
    AST_RETURN,

    /* declarations */
    AST_FUNCTION,
    AST_PARAM,
    AST_LET,

    /* type */
    AST_TYPE_NAME,
    AST_TYPE_POINTER,
    AST_TYPE_SLICE,
    AST_TYPE_ARRAY,

    AST_STRUCT,
    AST_UNION,
} AstKind;

struct AstNode {
    AstKind kind;
    Span *span;
    Token *trivia;
    AstNode *last;
    AstNode *next;

    union {
        /* AST_IDENT */
        struct {
            const char *name;
            Info *info;
        } ident;

        /* AST_INT, AST_FLOAT, AST_STRING, AST_CHAR, */
        size_t int_val;
        float float_val;
        const char *str;
        char char_val;
        bool bool_val;

        /* AST_UNARY */
        struct {
            TokenKind op;
            bool postfix;
            AstNode *operand;
        } unary;

        /* AST_BINARY */
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

        /* AST_MEMBER */
        struct {
            AstNode *left;
            AstNode *member;
        } member;

        /* AST_CALL: args is a linked list */
        struct {
            const char *callee;
            AstNode *args;
            size_t argc;
        } call;

        /* AST_BLOCK: stmts is a linked list */
        /* AST_EXPR_STMT, AST_RETURN, AST_ELSE, AST_LOOP */
        struct {
            AstNode *expr;
        } stmt;

        /* AST_STRUCT, AST_UNION */
        struct {
            const char *name;
            AstNode *body;
        } structure;

        /* AST_LET */
        struct {
            const char *name;
            AstNode *type;
            AstNode *expr;
            bool mutable;
        } let;

        /* AST_FUNCTION */
        struct {
            bool noreturn;
            bool public;
            const char *name;
            AstNode *params;
            size_t nparams;
            AstNode *body;
            AstNode *return_type;
        } func;

        /* AST_TYPE_NAME */
        Token *type_tok;

        /* AST_TYPE_ARRAY */
        struct {
            AstNode *inner;
            AstNode *len;
        } array;

        /* AST_TYPE_POINTER, AST_TYPE_SLICE */
        struct {
            AstNode *inner;
            bool mutable;
        } pointer;

        /* AST_PARAM */
        struct {
            AstNode *type;
            const char *name;
        } param;
    } as;
};

AstNode *ast_new(Arena *arena, Span *span, Token *trivia);

AstNode *ast_ident_val(AstNode *base, const char *name);
AstNode *ast_int_val(AstNode *base, size_t int_val);
AstNode *ast_float_val(AstNode *base, float float_val);
AstNode *ast_char_val(AstNode *base, char char_val);
AstNode *ast_string_val(AstNode *base, const char *str);

AstNode *ast_unary(AstNode *base, TokenKind op, bool postfix, AstNode *operand);
AstNode *ast_binary(AstNode *base, TokenKind op, AstNode *left, AstNode *right);
AstNode *ast_index(AstNode *base, AstNode *left, AstNode *index);
AstNode *ast_member(AstNode *base, AstKind kind, AstNode *left, AstNode *member);
AstNode *ast_call(AstNode *base, const char *callee, AstNode *args, size_t argc);

AstNode *ast_stmt(AstNode *base, AstKind kind, AstNode *expr);
AstNode *ast_let(AstNode *base, const char *name, AstNode *type, AstNode *expr,
                 bool mutable);
AstNode *ast_cond(AstNode *base, AstKind kind, AstNode *cond, AstNode *then_b,
                  AstNode *else_b);
AstNode *ast_ctl(AstNode *base, AstKind kind, AstNode *cond, AstNode *body);
AstNode *ast_structure(AstNode *base, AstKind kind, const char *name, AstNode *body);

AstNode *ast_type(AstNode *base, Token *type);
AstNode *ast_array(AstNode *base, AstNode *inner, AstNode *len);
AstNode *ast_pointer(AstNode *base, AstKind kind, AstNode *inner, bool mutable);

AstNode *ast_function(AstNode *base, bool noreturn, bool public, const char *name,
                      AstNode *params, size_t nparams, AstNode *body,
                      AstNode *return_type);
AstNode *ast_param(AstNode *base, const char *name, AstNode *type);

void print_ast(AstNode *root);
void format_ast(FILE *file, AstNode *root);
