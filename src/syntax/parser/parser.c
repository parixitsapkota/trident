#include <stdbool.h>

#include "parser.h"
#include "process.h"

#include "core/diags/diags.h"
#include "core/mem/mem.h"
#include "core/source/source.h"
#include "syntax/ast/ast.h"
#include "syntax/token/token.h"

Parser *init_parser(Source *src, Token *tokens) {
    Parser *p = xmalloc(sizeof(Parser));
    *p = (Parser){0};
    p->src = src;
    p->tokens = tokens;
    p->tok_tail = tokens;
    p->nodes = init_arena(sizeof(AstNode) * 1024);
    return p;
}

AstNode *parse_any(Parser *p) {
    switch (current(p)->kind) {
    case PUNCT_SEMICOLON: pconsume(p); return NULL;
    case PUNCT_O_BRACE: return parse_block_stmt(p);
    case KW_LET: return parse_let_stmt(p);
    case KW_FUNC: return parse_func_stmt(p, false);
    case KW_RETURN: return parse_expr_stmt(p, AST_RETURN);
    case KW_BREAK: return parse_expr_stmt(p, AST_BREAK);
    case KW_CONTINUE: return parse_expr_stmt(p, AST_CONTINUE);
    default: {
        AstNode *expr = parse_expr(p, PREC_NONE);
        expect_and_consume(p, PUNCT_SEMICOLON);
        return expr;
    }
    }
}

AstNode *parse(Parser *p) {
    NodeChain chain = {0};

    for (;;) {
        if (is_kind(p, END_OF_TOKEN)) break;
        AstNode *node = parse_any(p);
        if (node != NULL) chain_append(&chain, node);
    }

    AstNode *base = ast_new(p->nodes, NULL, NULL);
    p->root = ast_stmt(base, AST_PROGRAM, chain.first);
    if (p->errorc > 0) fatal("parser had %zu error(s).", p->errorc);
    return p->root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
