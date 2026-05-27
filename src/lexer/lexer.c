#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

#include "../token/token.h"
#include "../utils/utils.h"

char *get_word(const char *buffer, size_t *i, size_t *col) {
  const size_t start = *i;
  while (isalnum(buffer[*i]) || buffer[*i] == '_') {
    ++*col;
    ++*i;
  }
  return s_substr(buffer, start, *i);
}

char *get_digit(const char *buffer, size_t *i, int *is_float, size_t *col) {
  const size_t start = *i;
  *is_float = 0;
  if (buffer[*i] == '.') {
    *is_float = 1;
    ++*col;
    ++*i;
  }
  while (isdigit(buffer[*i]) || buffer[*i] == '.') {
    if (buffer[*i] == '.') {
      if (*is_float) {
        break;
      }
      *is_float = 1;
    }
    ++*col;
    ++*i;
  }
  return s_substr(buffer, start, *i);
}

char *get_string(const char *buffer, size_t *i, size_t *col, TokenKind *error) {
  const size_t start = ++*i; // Skip char `"`.
  ++*col;
  while (buffer[*i] != '"') {
    if (buffer[*i] == '\0' || buffer[*i] == '\n') {
      *error = UNTERMINATED_STRING;
      break;
    }
    ++*col;
    ++*i;
  }
  if (buffer[*i] == '"') {
    ++*i; // Skip char `"`.
    ++*col;
  }
  return s_substr(buffer, start, *i - 1);
}

Tokens lexer(const char *buffer) {
  Tokens tokens = {0};
  size_t line = 1, col = 1;
  size_t i = 0;

  while (buffer[i] != '\0') {

    // Skip line comment
    if (buffer[i] == '/' && buffer[i + 1] == '/') {
      while (buffer[i] != '\n') {
        if (buffer[i] == '\0') {
          break;
        }
        ++col;
        ++i;
      }
      continue;
    }

    // Handle whitespace.
    if (isspace(buffer[i])) {
      if (buffer[i] == '\n') {
        col = 1;
        ++line;
      } else {
        ++col;
      }
      ++i;
      continue;
    }

    const size_t tcol = col;

    // Handle identifiers & keywords.
    if (isalpha(buffer[i]) || buffer[i] == '_') {
      char *word = get_word(buffer, &i, &col);
      const TokenKind kind = get_keyword_kind(word);
      if (kind != IDENTIFIER) {
        s_free(word);
        add_token(&tokens, kind, NULL, line, tcol);
        continue;
      }
      add_token(&tokens, kind, word, line, tcol);
      continue;
    }

    // Handle digits.
    if (isdigit(buffer[i]) || (buffer[i] == '.' && isdigit(buffer[i + 1]))) {
      int is_float = 0;
      const char *digit = get_digit(buffer, &i, &is_float, &col);
      const TokenKind kind = is_float ? FLOAT : INT;
      add_token(&tokens, kind, digit, line, tcol);
      continue;
    }

    // Handle strings.
    if (buffer[i] == '"') {
      TokenKind error = UNKNOWN;
      const char *string = get_string(buffer, &i, &col, &error);
      if (error != UNKNOWN) {
        add_token(&tokens, error, NULL, line, tcol);
        continue;
      }
      add_token(&tokens, STRING, string, line, tcol);
      continue;
    }

    // Handle directives.
    if (buffer[i] == '@' && isalpha(buffer[i + 1])) {
      ++i; // Skip `@` char.
      ++col;
      const char *directive = get_word(buffer, &i, &col);
      TokenKind kind = get_directive_kind(directive);
      if (kind != UNKNOWN) {
        add_token(&tokens, kind, directive, line, tcol);
        continue;
      }
      add_token(&tokens, UNKNOWN_DIRECTIVE, directive, line, tcol);
      continue;
    }

// Handle operators and seperators.
#define add_s(a, b)                                                            \
  case a:                                                                      \
    add_token(&tokens, b, NULL, line, tcol);                                   \
    break

#define add_2s(a, b, c, d)                                                     \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      add_token(&tokens, d, NULL, line, tcol);                                 \
      ++col;                                                                   \
      ++i;                                                                     \
    } else {                                                                   \
      add_token(&tokens, c, NULL, line, tcol);                                 \
    }                                                                          \
    break

#define add_2sc(a, b, c, d, e)                                                 \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      add_token(&tokens, d, NULL, line, tcol);                                 \
      ++col;                                                                   \
      ++i;                                                                     \
    } else if (buffer[i + 1] == a) {                                           \
      add_token(&tokens, e, NULL, line, tcol);                                 \
      ++col;                                                                   \
      ++i;                                                                     \
    } else {                                                                   \
      add_token(&tokens, c, NULL, line, tcol);                                 \
    }                                                                          \
    break

    switch (buffer[i]) {
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
      // Handle operators with 2 char.
      add_2s('*', '=', STAR, STAR_ASSIGN);
      add_2s('%', '=', PERCENT, PERCENT_ASSIGN);
      add_2s('/', '=', SLASH, SLASH_ASSIGN);
      add_2s('=', '=', ASSIGN, EQUAL);
      add_2s('!', '=', NEGATE, NOT_EQUAL);
      add_2s('&', '&', UNKNOWN_TOKEN, LOGICAL_AND);
      add_2s('|', '|', UNKNOWN_TOKEN, LOGICAL_OR);
      // Handle operators with 2 char, 3 cases.
      add_2sc('+', '=', PLUS, PLUS_ASSIGN, INCREMENT);
      add_2sc('-', '=', DASH, DASH_ASSIGN, DECREMENT);
      add_2sc('<', '=', LESSER, LESSER_EQUAL, SHIFT_LEFT);
      add_2sc('>', '=', GREATER, GREATER_EQUAL, SHIFT_RIGHT);
    default:
      add_token(&tokens, UNKNOWN_TOKEN, NULL, line, tcol);
    }

#undef add_s
#undef add_2s
#undef add_2sc

    ++col;
    ++i;
  }
  putc('\n', stdout);
  return tokens;
}
