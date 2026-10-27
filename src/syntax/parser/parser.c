#include "parser.h"
#include "core/source.h"
#include "process.h"

#include "core/diags.h"
#include "core/mem.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"
#include <stdbool.h>

Parser *init_parser(Source *src, Token *tokens) {
    Parser *p = xmalloc(sizeof(Parser));
    *p = (Parser){0};
    p->src = src;
    p->tokens = tokens;
    p->tok_tail = tokens;
    p->nodes = init_arena(sizeof(AstNode) * 1024);
    return p;
}

AstNode *parse(Parser *p) {
    NodeChain chain = {0};

    for (;;) {
        AstNode *node = NULL;

        switch (current(p)->kind) {
        case END_OF_TOKEN: goto done;
        case KW_LET: node = parse_let_stmt(p); break;
        case KW_FUNC: node = parse_func_stmt(p, false); break;
        case PUNCT_SEMICOLON: pconsume(p); continue;
        default: {
            Span *start = current(p)->span;
            (void)parse_expr(p, PREC_NONE);
            Span *span = span_merge(start, current(p)->span);
            parser_error(p, span, "expected a declaration!");
            continue;
        }
        }

        if (node != NULL) chain_append(&chain, node);
    }

done:
    p->root = chain.first;
    if (p->errorc > 0) fatal("parser had %zu error(s).", p->errorc);
    return p->root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
