#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "core/source/source.h"
#include "parser.h"

#include "syntax/ast/ast.h"
#include "syntax/parser/process.h"
#include "syntax/token/token.h"

AstNode *parse_decl_stmt(Parser *p, bool mutable) {
    Token *first_tok = current(p);
    const char *name = expect_and_consume(p, IDENTIFIER_LIT)->lexeme;
    expect_and_consume(p, PUNCT_COLON);

    AstNode *type = parse_type(p);

    AstNode *expr = NULL;
    if (is_kind(p, PUNCT_ASSIGN)) {
        pconsume(p); // `='
        expr = parse_expr(p, PREC_NONE);
        expect_and_consume(p, PUNCT_SEMICOLON);
        if (!expr) return NULL;
    }

    Span *span = span_merge(first_tok->span, current(p)->last->span);
    AstNode *base = ast_new(p->nodes, span, first_tok);
    return ast_let(base, name, type, expr, mutable);
}

AstNode *parse_expr_stmt(Parser *p, AstKind kind) {
    Token *start = pconsume(p);
    AstNode *expr = parse_expr(p, PREC_NONE);
    Span *span = span_merge(start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_stmt(base, kind, expr);
}

AstNode *parse_func_param(Parser *p) {
    Token *start = current(p);
    const char *name = NULL;
    if (is_kind(p, IDENTIFIER_LIT)) name = pconsume(p)->lexeme;
    if (!name)
        parser_error(p, NULL, "expected a name, but got `%s'",
                     token_kind_to_str(current(p)->kind));

    expect_and_consume(p, PUNCT_COLON);

    AstNode *type = parse_type(p);
    Span *span = span_merge(start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, start);
    return ast_param(base, name, type);
}

static inline bool params_end(Parser *p) {
    return is_kind(p, PUNCT_COMMA) || is_kind(p, PUNCT_C_PREN);
}

static inline bool params_sync(Parser *p) {
    return params_end(p) || is_kind(p, PUNCT_COLON) || is_kind(p, PUNCT_ARROW) ||
           is_kind(p, END_OF_TOKEN);
}

AstNode *parse_func_params(Parser *p, size_t *nparams) {
    NodeChain params = {0};
    *nparams = 0;

    if (is_kind(p, PUNCT_C_PREN)) return NULL;

    do {
        AstNode *param = parse_func_param(p);
        if (param) {
            chain_append(&params, param);
            ++*nparams;
            if (!params_end(p))
                parser_error(p, NULL, "unexpected token `%s'",
                             token_kind_to_str(current(p)->kind));
        }
        while (!params_sync(p))
            pconsume(p);

    } while (is_kind(p, PUNCT_COMMA) && pconsume(p));

    return params.first;
}

AstNode *parse_func_stmt(Parser *p, bool public) {
    bool noreturn = false;
    AstNode *type = NULL;
    Span *start = pconsume(p)->span;
    const char *name = pconsume(p)->lexeme;

    expect_and_consume(p, PUNCT_O_PREN);
    size_t nparams = 0;
    AstNode *params = parse_func_params(p, &nparams);
    expect_and_consume(p, PUNCT_C_PREN);

    if (current(p)->kind == IC_NORETURN) {
        pconsume(p);
        noreturn = true;
    } else {
        expect_and_consume(p, PUNCT_ARROW);
        type = parse_type(p);
    }

    AstNode *body = parse_any(p);
    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_function(base, noreturn, public, name, params, nparams, body, type);
}

AstNode *parse_block_stmt(Parser *p) {
    Span *start = pconsume(p)->span;

    NodeChain stmts = {0};
    while (current(p) && current(p)->kind != PUNCT_C_BRACE) {
        Token *before = current(p);
        chain_append(&stmts, parse_any(p));

        if (current(p) == before && current(p)) pconsume(p);
    }
    expect_and_consume(p, PUNCT_C_BRACE);

    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_stmt(base, AST_BLOCK, stmts.first);
}

AstNode *parse_structure_stmt(Parser *p, AstKind kind) {
    Span *start = pconsume(p)->span;

    const char *name = expect_and_consume(p, IDENTIFIER_LIT)->lexeme;
    AstNode *body = parse_any(p);
    expect_and_consume(p, PUNCT_SEMICOLON);

    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_structure(base, kind, name, body);
}

AstNode *parse_if_stmt(Parser *p) {
    Token *if_start = pconsume(p);
    AstNode *cond = parse_paren_expr(p);
    AstNode *then_b = parse_any(p);

    AstNode *chain = NULL;
    if (is_kind(p, KW_ELSE)) {
        Token *else_start = pconsume(p);
        if (is_kind(p, KW_IF)) {
            chain = parse_if_stmt(p);
            chain->kind = AST_ELSE_IF;
        } else {
            AstNode *else_body = parse_any(p);
            Span *span = span_merge(else_start->span, current(p)->span);
            AstNode *base = ast_new(p->nodes, span, else_start);
            chain = ast_stmt(base, AST_ELSE, else_body);
        }
    }
    Span *span = span_merge(if_start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, if_start);
    return ast_cond(base, AST_IF, cond, then_b, chain);
}

AstNode *parse_while_stmt(Parser *p) {
    Token *while_start = pconsume(p);
    AstNode *cond = parse_paren_expr(p);
    AstNode *then_b = parse_any(p);
    Span *span = span_merge(while_start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, while_start);
    return ast_ctl(base, AST_WHILE, cond, then_b);
}

AstNode *parse_loop_stmt(Parser *p) {
    Token *loop_start = pconsume(p);
    AstNode *body = parse_any(p);
    Span *span = span_merge(loop_start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, loop_start);
    return ast_stmt(base, AST_LOOP, body);
}
