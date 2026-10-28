#pragma once

#include "syntax/token/token.h"

#include "core/source/source.h"

typedef struct Lexer Lexer;

Lexer *init_lexer(Source *src);
Token *lex(Lexer *l, bool lex_all, const char *buffer);
void free_lexer(Lexer *l);
