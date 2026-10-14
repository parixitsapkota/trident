#include "core/diags.h"
#include "core/source.h"
#include "syntax/lexer/lexer.h"
#include "syntax/token/token.h"
#include <stdbool.h>

int main(int argc, const char *const *argv) {
    if (argc != 2)
        fatal("incorrect usage! \nCorrect usage : %s <file_pat> <output>", argv[0]);
    Source *src = init_source(argv[1]);
    Lexer *l = init_lexer(src);
    Token *tokens = lex(l, true);
    print_tokens(argv[1], tokens);
    free_lexer(l);
    free_source(src);
}
