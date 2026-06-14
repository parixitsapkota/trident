#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
