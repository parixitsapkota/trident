#include <stdbool.h>

#include "core/diags.h"
#include "core/mem.h"
#include "core/source.h"
#include "syntax/lexer/lexer.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

int main(int argc, const char *const *argv) {
    if (argc != 2) {
        fatal("incorrect usage! \n");
        fatal("Correct usage : %s <file_pat>", argv[0]);
    }
    Source *src = init_source(argv[1]);
    Lexer *l = init_lexer(src);
    Token *tokens = lex(l, false);
    (void)tokens;

    Arena *a = init_arena(sizeof(AstNode) * 1024);
    AstNode *one = ast_int_val(a, NULL, 1);
    AstNode *root = ast_binary(a, NULL, AST_BINARY, PUNCT_ADD, one, one);
    print_ast(root);
    free_arena(a);

    free_lexer(l);
    free_source(src);
}
