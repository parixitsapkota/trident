#include "parser.h"

#include "core/mem.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

struct Parser {
    Source *src;
    Token *tokens;
    Arena *nodes;
};

Parser *init_parser(Source *src, Token *tokens) {
    Parser *p = xmalloc(sizeof(Parser));
    Arena *nodes = init_arena(sizeof(AstNode) * 1024);
    *p = (Parser){src, tokens, nodes};
    return p;
}

AstNode *parse(Parser *p) {
    Arena *a = p->nodes;
    AstNode *one = ast_new(a, NULL, NULL);
    AstNode *root = ast_new(a, NULL, NULL);
    ast_int_val(one, 1);
    ast_binary(root, PUNCT_ADD, one, one);
    return root;
}

void free_Parser(Parser *p) {
    free_arena(p->nodes);
    free(p);
}
