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
    bool mutable = parse_constness(p);
    switch (current(p)->kind) {
    case PUNCT_SEMICOLON: pconsume(p); return NULL;
    case PUNCT_O_BRACE: return parse_block_stmt(p);
    case KW_STRUCT: return parse_structure_stmt(p, AST_STRUCT);
    case KW_UNION: return parse_structure_stmt(p, AST_UNION);
    case KW_FUNC: return parse_func_stmt(p, false);
    case KW_RETURN: return parse_expr_stmt(p, AST_RETURN);
    case KW_BREAK: return parse_expr_stmt(p, AST_BREAK);
    case KW_CONTINUE: return parse_expr_stmt(p, AST_CONTINUE);
    case IDENTIFIER_LIT:
        if (current(p)->next) {
            if (current(p)->next->kind == PUNCT_COLON) {
                return parse_decl_stmt(p, mutable);
            }
        }
        [[fallthrough]];
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
    if (p->errorc > 0) {
        putc('\n', stderr);
        print_ast(p->root);
        putc('\n', stderr);
        fatal("parser had %zu error(s).", p->errorc);
    }
    return p->root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
