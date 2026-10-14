#pragma once

#include "core/span.h"
#include <stddef.h>

typedef enum {
#define LITERAL(tok) tok##_LIT,
#define KEYWORD(tok, name) KW_##tok,
#define PUNCTUATION(tok, name) PUNCT_##tok,
#define TOKEN(tok, name) tok,
#include "token.def"
#undef LITERAL
#undef KEYWORD
#undef PUNCTUATION
#undef TOKEN
} TokenKind;

typedef union {
    size_t i;
    double f;
} NumValue;

typedef struct Token {
    TokenKind kind;
    const char *lexeme;
    Span *span;
    NumValue val;
    struct Token *next;
} Token;

const char *token_kind_to_str(TokenKind kind);
Token *token(Arena *arena, TokenKind kind, const char *lexeme, Span *span, NumValue val);
void print_tokens(const char *file_name, const Token *tokens);
