#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../SHI/shi_opa.h"
#include "lexer.h"

#define tokstr(x)                                                              \
  case x:                                                                      \
    return #x
char *token_kind_to_str(const TokenKind kind) {
  switch (kind) {
    tokstr(UNTERMINATED_STRING);
    tokstr(UNKNOWN_TOKEN);
    tokstr(UNKNOWN_DIRECTIVE);
  default:
    return "?TOKEN?";
  }
}

static int compare_keywords(const void *a, const void *b) {
  const char *search_word = a;
  const KeywordMap *map_item = b;
  return strcmp(search_word, map_item->word);
}

TokenKind get_keyword_kind(const char *word) {
  if (!word || *word == '\0') {
    return IDENTIFIER;
  }
  const KeywordMap *match =
      bsearch(word, keywords, sizeof(keywords) / sizeof(KeywordMap),
              sizeof(KeywordMap), compare_keywords);
  return match ? match->kind : IDENTIFIER;
}

TokenKind get_directive_kind(const char *word) {
  if (!strcmp("typeof", word)) {
    return TYPEOF;
  }
  if (!strcmp("define", word)) {
    return DEFINE;
  }
  if (!strcmp("import", word)) {
    return IMPORT;
  }
  return UNKNOWN;
}

bool is_token_kind_error(const TokenKind kind) {
  switch (kind) {
  case UNKNOWN:
  case UNTERMINATED_STRING:
  case UNKNOWN_TOKEN:
  case UNKNOWN_DIRECTIVE:
    return 1;
  default:
    return 0;
  }
}

size_t print_error_token_kind(const char *file_name, SHI_OPA *token_pool) {
  size_t errors = 0;
  for (size_t i = 0;; ++i) {
    Token *token = shi_opa_index(token_pool, i);
    if (is_token_kind_error(token->kind)) {
      ++errors;
      fprintf(stderr, "%s:%lu:%lu: %s\n", file_name, token->line, token->col,
              token_kind_to_str(token->kind));
    }
    if (token->kind == END_OF_TOKEN)
      break;
  }
  if (errors)
    fprintf(stderr, "%lu ERRORS!\n", errors);
  return errors;
}

char *substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

char *get_word(const char *buffer, size_t *i, size_t *col) {
  const size_t start = *i;
  while (isalnum(buffer[*i]) || buffer[*i] == '_') {
    ++*col;
    ++*i;
  }
  return substr(buffer, start, *i);
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
  return substr(buffer, start, *i);
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
  return substr(buffer, start, *i - 1);
}

SHI_OPA *lexer(const char *buffer) {

  SHI_OPA *token_pool_head = shi_opa_init(Token, 1024);
  SHI_OPA *token_pool = token_pool_head;

  size_t line = 1, col = 1;
  size_t i = 0;
  Token c_token = {0};

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
        free(word);
        c_token = (Token){kind, NULL, line, tcol};
      } else {
        c_token = (Token){kind, word, line, tcol};
      }
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle digits.
    if (isdigit(buffer[i]) || (buffer[i] == '.' && isdigit(buffer[i + 1]))) {
      int is_float = 0;
      const char *digit = get_digit(buffer, &i, &is_float, &col);
      const TokenKind kind = is_float ? FLOAT : INT;
      c_token = (Token){kind, (char *)digit, line, tcol};
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle strings.
    if (buffer[i] == '"') {
      TokenKind error = UNKNOWN;
      const char *string = get_string(buffer, &i, &col, &error);
      if (error != UNKNOWN) {
        c_token = (Token){error, NULL, line, tcol};
      } else {
        c_token = (Token){STRING, (char *)string, line, tcol};
      }
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle directives.
    if (buffer[i] == '@' && isalpha(buffer[i + 1])) {
      ++i; // Skip `@` char.
      ++col;
      const char *directive = get_word(buffer, &i, &col);
      TokenKind kind = get_directive_kind(directive);
      if (kind != UNKNOWN) {
        c_token = (Token){kind, (char *)directive, line, tcol};
      } else {
        c_token = (Token){UNKNOWN_DIRECTIVE, (char *)directive, line, tcol};
      }
      shi_opa_push(token_pool, c_token);
      continue;
    }

// Handle operators and seperators.
#define incLC                                                                  \
  ++col;                                                                       \
  ++i

#define add_s(a, b)                                                            \
  case a:                                                                      \
    c_token = (Token){b, NULL, line, tcol};                                    \
    break

#define add_2s(a, b, c, d)                                                     \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      c_token = (Token){d, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else {                                                                   \
      c_token = (Token){c, NULL, line, tcol};                                  \
    }                                                                          \
    break

#define add_2sc(a, b, c, d, e)                                                 \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      c_token = (Token){d, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else if (buffer[i + 1] == a) {                                           \
      c_token = (Token){e, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else {                                                                   \
      c_token = (Token){c, NULL, line, tcol};                                  \
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
      c_token = (Token){UNKNOWN_TOKEN, NULL, line, tcol};
    }

    shi_opa_push(token_pool, c_token);
    incLC;
  }

  c_token = (Token){END_OF_TOKEN, NULL, 0, 0};
  shi_opa_push(token_pool, c_token);
  return token_pool_head;
}
