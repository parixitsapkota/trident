#include <stdbool.h>

#include "core/diags/diags.h"
#include "core/source/source.h"
#include "syntax/ast/ast.h"
#include "syntax/lexer/lexer.h"
#include "syntax/parser/parser.h"
#include "syntax/token/token.h"

int main(int argc, const char *const *argv) {
    if (argc != 2) {
        fatal("incorrect usage! \n");
        fatal("Correct usage : %s <file_pat>", argv[0]);
    }

    Source *src = init_source(argv[1]);
    Lexer *l = init_lexer(src);
    Token *tokens = lex(l, false, NULL);
    Parser *p = init_parser(src, tokens);
    AstNode *root = parse(p);
    print_ast(root);

    free_Parser(p);
    free_lexer(l);
    free_source(src);
}
