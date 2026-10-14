#pragma once

#include "core/source.h"
#include "syntax/token/token.h"

typedef struct Lexer Lexer;

Lexer *init_lexer(Source *src);
Token *lex(Lexer *l, bool lex_all);
void free_lexer(Lexer *l);
