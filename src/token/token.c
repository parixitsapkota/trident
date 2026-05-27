#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "token.h"

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

void add_token(Tokens *self, const TokenKind kind, const char *value,
               const size_t line, const size_t col) {
  const int alloc1024 = 1024;
  if (!self->tokens) {
    self->token_capacity = alloc1024;
    self->tokens = s_malloc(sizeof(Token) * self->token_capacity);
  } else if (self->token_capacity <= self->token_count) {
    self->token_capacity += alloc1024;
    self->tokens =
        s_realloc(self->tokens, sizeof(Token) * self->token_capacity);
  }
  self->tokens[self->token_count] = (Token){kind, (char *)value, line, col};
  ++self->token_count;
}

bool is_error_token_kind(const TokenKind kind) {
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

void print_error_tokens(const Tokens *self) {
  for (size_t i = 0; i <= self->token_count; ++i) {
    if (is_error_token_kind(self->tokens[i].kind)) {
      char *msg = token_kind_to_str(self->tokens[i].kind);
      const size_t line = self->tokens[i].line;
      const size_t col = self->tokens[i].col;
      fprintf(stderr, "%s at %lu:%lu\n", msg, line, col);
    }
  }
}

void free_tokens(Tokens *self) {
  for (size_t i = 0; i <= self->token_count; ++i) {
    if (self->tokens[i].value != NULL) {
      s_free(self->tokens[i].value);
      self->tokens[i].value = NULL;
    }
  }
  s_free(self->tokens);
  self->tokens = NULL;
}
