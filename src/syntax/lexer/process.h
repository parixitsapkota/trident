#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "lexer.h"

struct Lexer {
    const Source *src;
    const char *buffer;
    size_t buf_len;
    size_t i;
    size_t ln;
    size_t cn;
    size_t t_cn;
    Token *token_head;
    Token *token_tail;
    Arena *lexeme;
    Arena *token;
    Arena *spans;
    size_t errorc;
};

static inline bool is_space(char c) { return isspace((unsigned char)c) != 0; }
static inline bool is_alpha(char c) { return isalpha((unsigned char)c) != 0; }
static inline bool is_digit(char c) { return isdigit((unsigned char)c) != 0; }
static inline bool is_xdigit(char c) { return isxdigit((unsigned char)c) != 0; }
static inline bool is_bin_digit(char c) { return c == '0' || c == '1'; }
static inline bool is_octal(char c) { return c >= '0' && c <= '7'; }
static inline bool is_ident_char(char c) {
    return isalnum((unsigned char)c) != 0 || c == '_';
}

bool starts_number(Lexer *l);

static inline bool is_at_end(const Lexer *l) { return l->i >= l->buf_len; }

char peek(const Lexer *l, size_t offset);
void consume(Lexer *l, size_t count);
void lexer_error(Lexer *l, Location start, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));
void lexer_emit(Lexer *l, Location start, Location end, TokenKind kind,
                const char *lexeme, NumValue val);

size_t consume_while(Lexer *l, bool (*pred)(char));
size_t decode_escapes(Lexer *l, Location start, char *s, size_t len);
void lex_number(Lexer *l, Location start);
void lex_intrinsic(Lexer *l, Location start);
