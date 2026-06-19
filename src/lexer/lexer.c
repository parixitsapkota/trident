#include "lexer.h"
#include "../../SHI/shi_hs.h"
#include "../../SHI/shi_opa.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

char *get_word(Lexer *l) {
  const size_t start = l->index;
  while (isalnum(peak(l, 0)) || peak(l, 0) == '_')
    consume(l);
  return substr(l->buffer, start, l->index);
}

// TODO : add support for escape characters.
char *get_string(Lexer *l, TokenKind *error, char c) {
  consume(l); // Skip first char.
  const size_t start = l->index;

  while (peak(l, 0) != c) {
    if (peak(l, 0) == '\0' || peak(l, 0) == '\n') {
      *error = UNTERMINATED_STRING;
      break;
    }
    consume(l);
  }
  if (peak(l, 0) == c)
    consume(l); // Skip last char.
  return substr(l->buffer, start, l->index - 1);
}

char *get_string_ident(Lexer *l, TokenKind *error) {
  consume(l); // Skip char `"`.
  const size_t start = l->index;

  while (peak(l, 0) != '"') {
    if (peak(l, 0) == '\0' || peak(l, 0) == '\n') {
      *error = UNTERMINATED_STRING;
      break;
    }
    consume(l);
  }
  if (peak(l, 0) == '"')
    consume(l); // Skip char `"`.
  return substr(l->buffer, start, l->index - 1);
}

char *intern_symbol(Lexer *l, char *word) {
  if (shi_hs_has(l->set, word)) {
    char *existing = (char *)shi_hs_get(l->set, word);
    free(word);
    return existing;
  }
  shi_hs_put(l->set, word, word);
  return word;
}

#define TOKEN_CAP 1024
Lexer init_lexer(const char *buffer) {
  Lexer l = {0};
  l.buffer = buffer;
  l.line = 1;
  l.col = 1;
  l.token_pool_head = shi_opa_init(Token, TOKEN_CAP);
  l.token_pool = l.token_pool_head;
  l.set = shi_hs_init(TOKEN_CAP);
  return l;
}

LexReturn lexer(const char *buffer) {

  Lexer l = init_lexer(buffer);
  Token c_token = {0};

  while (peak(&l, 0) != '\0') {

    // Skip line comment
    if (peak(&l, 0) == '/' && peak(&l, 1) == '/') {
      while (peak(&l, 0) != '\n') {
        if (peak(&l, 0) == '\0') {
          break;
        }
        consume(&l);
      }
      continue;
    }

    // Handle whitespace.
    if (isspace(peak(&l, 0))) {
      if (peak(&l, 0) == '\n') {
        l.col = 1;
        ++l.line;
      } else {
        ++l.col;
      }
      ++l.index;
      continue;
    }

    const size_t tcol = l.col;

    // Handle identifiers & keywords.
    if (isalpha(peak(&l, 0)) || peak(&l, 0) == '_') {
      size_t temp_i = l.index;
      char *word = get_word(&l);
      const TokenKind kind = get_keyword_kind(word, l.index - temp_i);
      if (kind != IDENTIFIER) {
        free(word);
        c_token = (Token){kind, NULL, l.line, tcol};
      } else {
        word = intern_symbol(&l, word);
        c_token = (Token){kind, word, l.line, tcol};
      }
      shi_opa_push(l.token_pool, c_token);
      continue;
    }

    // Handle digits.
    if (isdigit(peak(&l, 0)) || (peak(&l, 0) == '.' && isdigit(peak(&l, 1)))) {
      TokenKind kind = UNKNOWN;
      char *digit = get_digit(&l, &kind);
      digit = intern_symbol(&l, digit);
      c_token = (Token){kind, digit, l.line, tcol};
      shi_opa_push(l.token_pool, c_token);
      continue;
    }

    // Handle strings & char.
    if (peak(&l, 0) == '"' || peak(&l, 0) == '\'') {
      TokenKind error = peak(&l, 0) == '"' ? STRING : CHARACTER;
      char *value = get_string(&l, &error, peak(&l, 0));
      value = intern_symbol(&l, value);
      c_token = (Token){error, value, l.line, tcol};
      shi_opa_push(l.token_pool, c_token);
      continue;
    }

    // Handle directives.
    if (peak(&l, 0) == '@') {
      if (peak(&l, 1) == '"' || isalpha(peak(&l, 1))) {
        consume(&l);
      }
      if (peak(&l, 0) == '"') {
        TokenKind error = IDENTIFIER;
        char *ident = get_string_ident(&l, &error);
        ident = intern_symbol(&l, ident);
        c_token = (Token){error, ident, l.line, tcol};
      } else if (isalpha(peak(&l, 0))) {
        char *directive = get_word(&l);
        TokenKind kind = get_directive_kind(directive);
        free(directive);
        kind = kind != UNKNOWN ? kind : UNKNOWN_DIRECTIVE;
        c_token = (Token){kind, NULL, l.line, tcol};
      } else {
        break;
      }
      shi_opa_push(l.token_pool, c_token);
      continue;
    }

// Handle operators and seperators.
#define add_s(a, b)                                                            \
  case a:                                                                      \
    c_token = (Token){b, NULL, l.line, tcol};                                  \
    break

#define add_2s(a, b, c, d)                                                     \
  case a:                                                                      \
    if (peak(&l, 1) == b) {                                                    \
      c_token = (Token){d, NULL, l.line, tcol};                                \
      consume(&l);                                                             \
    } else {                                                                   \
      c_token = (Token){c, NULL, l.line, tcol};                                \
    }                                                                          \
    break

#define add_2sc(a, b, c, d, e)                                                 \
  case a:                                                                      \
    if (peak(&l, 1) == b) {                                                    \
      c_token = (Token){d, NULL, l.line, tcol};                                \
      consume(&l);                                                             \
    } else if (peak(&l, 1) == a) {                                             \
      c_token = (Token){e, NULL, l.line, tcol};                                \
      consume(&l);                                                             \
    } else {                                                                   \
      c_token = (Token){c, NULL, l.line, tcol};                                \
    }                                                                          \
    break

#define add_3sc(a, b, c, d, e, f)                                              \
  case a:                                                                      \
    if (peak(&l, 1) == b) {                                                    \
      c_token = (Token){d, NULL, l.line, tcol};                                \
      consume(&l);                                                             \
    } else if (peak(&l, 1) == a) {                                             \
      if (peak(&l, 2) == b) {                                                  \
        c_token = (Token){f, NULL, l.line, tcol};                              \
        consume(&l);                                                           \
      } else {                                                                 \
        c_token = (Token){e, NULL, l.line, tcol};                              \
      }                                                                        \
      consume(&l);                                                             \
    } else {                                                                   \
      c_token = (Token){c, NULL, l.line, tcol};                                \
    }                                                                          \
    break

    switch (peak(&l, 0)) {
      // Handle operators with 1 char.
      add_s('{', O_BRACE);
      add_s('}', C_BRACE);
      add_s('[', O_BRACKET);
      add_s(']', C_BRACKET);
      add_s('(', O_PREN);
      add_s(')', C_PREN);
      add_s(',', COMMA);
      add_s(':', COLN);
      add_s(';', SEMICOLON);
      add_s('~', BITWISE_NOT);
      add_s('?', WHAT);
      // Handle operators with 2 char.
      add_2s('*', '=', STAR, STAR_ASSIGN);
      add_2s('%', '=', PERCENT, PERCENT_ASSIGN);
      add_2s('/', '=', SLASH, SLASH_ASSIGN);
      add_2s('=', '=', ASSIGN, EQUAL);
      add_2s('!', '=', NEGATE, NOT_EQUAL);
      add_2s('^', '=', BITWISE_XOR, XOR_ASSIGN);
      add_2s('.', '.', DOT, DOT_DOT);
      // Handle operators with 2 char, 3 cases.
      add_2sc('+', '=', PLUS, PLUS_ASSIGN, INCREMENT);
      add_2sc('-', '=', DASH, DASH_ASSIGN, DECREMENT);
      add_2sc('&', '=', ANDPERCENT, AND_ASSIGN, LOGICAL_AND);
      add_2sc('|', '=', BITWISE_OR, OR_ASSIGN, LOGICAL_OR);
      // Handle operators with 3 char, 3 cases.
      add_3sc('<', '=', LESSER, LESSER_EQUAL, SHIFT_LEFT, SHIFT_LEFT_ASSIGN);
      add_3sc('>', '=', GREATER, GREATER_EQUAL, SHIFT_RIGHT,
              SHIFT_RIGHT_ASSIGN);
    default:
      c_token = (Token){UNKNOWN_TOKEN, NULL, l.line, tcol};
    }

    shi_opa_push(l.token_pool, c_token);
    consume(&l);
  }

  c_token = (Token){END_OF_TOKEN, NULL, 0, 0};
  shi_opa_push(l.token_pool, c_token);
  return (LexReturn){l.token_pool_head, l.set};
}

char peak(Lexer *l, int offset) { return l->buffer[l->index + offset]; }

void consume(Lexer *l) {
  ++l->col;
  ++l->index;
}
