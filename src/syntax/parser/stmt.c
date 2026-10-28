#include "core/source.h"
#include "parser.h"

#include "syntax/parser/process.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"
#include "type/type.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdnoreturn.h>
#include <string.h>

AstNode *parse_let_stmt(Parser *p) {
    Span *start = pconsume(p)->span;
    Token *type = pconsume(p);
    AstNode *expr = parse_expr(p, PREC_NONE);
    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_let(base, expr, type);
}

AstNode *parse_expr_stmt(Parser *p, AstKind kind) {
    Token *start = pconsume(p);
    AstNode *expr = parse_expr(p, PREC_NONE);
    Span *span = span_merge(start->span, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_stmt(base, kind, expr);
}

AstNode *parse_func_param(Parser *p) {
    Token *type = NULL, *name_tok = NULL;

    if (is_primitive_kind(current(p)->kind) || is_kind(p, IDENTIFIER_LIT))
        type = pconsume(p);

    if (is_kind(p, IDENTIFIER_LIT)) name_tok = pconsume(p);

    if (!type && !name_tok) {
        parser_error(p, NULL, "expected a parameter before `%s'",
                     token_kind_to_str(current(p)->kind));
        return NULL;
    } else if (!type) {
        parser_error(p, NULL, "expected a parameter type before `%s'",
                     token_kind_to_str(current(p)->kind));
    } else if (!name_tok) {
        parser_error(p, NULL, "expected a parameter name before `%s'",
                     token_kind_to_str(current(p)->kind));
    }

    Token *start = type ? type : name_tok;
    Token *last = name_tok ? name_tok : type;
    Span *span = span_merge(start->span, last->span);
    AstNode *base = ast_new(p->nodes, span, last->next);
    return ast_param(base, name_tok ? name_tok->lexeme : NULL, type);
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
    bool _noreturn = false;
    Token *type = NULL;
    Span *start = pconsume(p)->span;
    const char *name = pconsume(p)->lexeme;

    expect_and_consume(p, PUNCT_O_PREN);
    size_t nparams = 0;
    AstNode *params = parse_func_params(p, &nparams);
    expect_and_consume(p, PUNCT_C_PREN);

    if (current(p)->kind == IC_NORETURN) {
        pconsume(p);
        _noreturn = true;
        goto parse_body;
    }

    expect_and_consume(p, PUNCT_ARROW);
    if (!is_primitive_kind(current(p)->kind)) {
        parser_error(p, current(p)->span, "expected a type identifier but got `%s'",
                     token_kind_to_str(current(p)->kind));
    } else {
        type = pconsume(p);
    }

parse_body:;
    AstNode *body = parse_stmt(p);
    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_function(base, _noreturn, public, name, params, nparams, body, type);
}

static AstNode *parse_block_stmt(Parser *p) {
    Span *start = pconsume(p)->span;

    NodeChain stmts = {0};
    while (current(p) && current(p)->kind != PUNCT_C_BRACE) {
        Token *before = current(p);
        chain_append(&stmts, parse_stmt(p));

        if (current(p) == before && current(p)) pconsume(p);
    }
    expect_and_consume(p, PUNCT_C_BRACE);

    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_stmt(base, AST_BLOCK, stmts.first);
}

AstNode *parse_stmt(Parser *p) {
    switch (current(p)->kind) {
    case PUNCT_SEMICOLON: return NULL;
    case KW_BREAK: return parse_expr_stmt(p, AST_BREAK);
    case KW_CONTINUE: return parse_expr_stmt(p, AST_CONTINUE);
    case KW_RETURN: return parse_expr_stmt(p, AST_RETURN);
    case KW_LET: return parse_let_stmt(p);
    case PUNCT_O_BRACE: return parse_block_stmt(p);
    default: {
        AstNode *expr = parse_expr(p, PREC_NONE);
        expect_and_consume(p, PUNCT_SEMICOLON);
        return expr;
    }
    }
}
