#include "parser.h"
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
    p->root = parse_expr(p, PREC_NONE);
    if (p->errorc > 0) fatal("parser had %zu error(s).", p->errorc);
    return p->root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
