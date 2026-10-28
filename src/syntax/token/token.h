#pragma once

#include <stddef.h>

#include "core/source/source.h"

typedef enum {
#define LITERAL(tok, name) tok##_LIT,
#define KEYWORD(tok, name) KW_##tok,
#define PUNCTUATION(tok, name) PUNCT_##tok,
#define TOKEN(tok, name) tok,
#define INTRINSIC(tok, name) IC_##tok,
#include "token.def"
#undef INTRINSIC
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
    struct Token *last;
    struct Token *next;
} Token;

Token *token(Arena *arena, TokenKind kind, const char *lexeme, Span *span, NumValue val);
const char *token_kind_to_str(TokenKind kind);
void print_tokens(const Token *tokens);
