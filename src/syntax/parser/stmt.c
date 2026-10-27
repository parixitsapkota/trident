#include "core/source.h"
#include "parser.h"

#include "syntax/parser/process.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdnoreturn.h>

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

AstNode *parse_func_stmt(Parser *p, bool public) {
    bool _noreturn = false;
    Token *type = NULL;
    Span *start = pconsume(p)->span;
    const char *name = pconsume(p)->lexeme;

    expect_and_consume(p, PUNCT_O_PREN);
    AstNode *params = NULL;
    size_t nparams = 0;
    // TODO: AstNode *params = parse_func_params(p, &nparams);
    expect_and_consume(p, PUNCT_C_PREN);

    if (current(p)->kind == IC_NORETURN) {
        pconsume(p);
        _noreturn = true;
        goto parse_body;
    }

    expect_and_consume(p, PUNCT_ARROW);
    if (!is_primitive_kind(current(p)->kind)) {
        parser_error(p, "expected a type identifier but got `%s'",
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
    case PUNCT_O_BRACE: return parse_block_stmt(p);
    default: {
        AstNode *expr = parse_expr(p, PREC_NONE);
        expect_and_consume(p, PUNCT_SEMICOLON);
        return expr;
    }
    }
}
