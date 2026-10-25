#include "parser.h"
#include "process.h"

#include "core/mem.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

Parser *init_parser(Source *src, Token *tokens) {
    Parser *p = xmalloc(sizeof(Parser));
    *p = (Parser){0};
    p->src = src;
    p->tokens = tokens;
    p->nodes = init_arena(sizeof(AstNode) * 1024);
    return p;
}

AstNode *parse(Parser *p) {
    Arena *a = p->nodes;
    AstNode *one = ast_new(a, NULL, NULL);
    AstNode *root = ast_new(a, NULL, NULL);
    p->root = root;
    ast_int_val(one, 1);
    ast_binary(root, PUNCT_ADD, one, one);

    if (p->errorc > 0) parser_error(p, "parser had %zu error(s).", p->errorc);
    return p->root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
