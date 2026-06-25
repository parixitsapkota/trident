#ifndef TRIDENT_LEXER_I_H
#define TRIDENT_LEXER_I_H

#include "error/error.h"
#include "lexer.h"
#include "token.h"

char peak(const Lexer *l, int offset);
void consume(Lexer *l);
void add_token(Lexer *l, Token t);

char *token_kind_to_str(TokenKind kind);

char *get_word(Lexer *l);
char *get_string(Lexer *l, Error *e, char c);
char *get_string_ident(Lexer *l, Error *e);
TokenKind get_symbol(Lexer *l, size_t len);
char *get_digit(Lexer *l, TokenKind *kind);

TokenKind get_keyword_kind(const char *str, size_t len);
TokenKind get_directive_kind(const char *word);

#endif // TRIDENT_LEXER_I_H
