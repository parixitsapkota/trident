#ifndef TRIDENT_LEXER_H
#define TRIDENT_LEXER_H

#include <stddef.h>

#include "error/error.h"
#include "shi_hs.h"
#include "shi_opa.h"
#include "token.h"

typedef struct {
  const char *buffer;
  size_t index;
  size_t line;
  size_t col;

  SHI_OPA *token_pool_head;
  SHI_OPA *token_pool;
  SHI_HS *map;
} Lexer;

Lexer *init_lexer(const char *buffer);
void lexer(Lexer *l, size_t len, Error *e);
void print_tokens(Lexer *l);
void free_tokens(Lexer *l);

#endif // TRIDENT_LEXER_H
