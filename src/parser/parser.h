#ifndef TRIDENT_PARSER_H
#define TRIDENT_PARSER_H

#include <stddef.h>

#include "lexer/lexer.h"
#include "shi_arena.h"
#include "shi_hs.h"
#include "shi_opa.h"

typedef struct {
  SHI_OPA *tokens_pool;
  size_t index;
  SHI_ARENA *ast;
} Parser;

Parser *init_parser(Lexer *l);
void parse(Parser *p);
void free_parser(Parser *p);

#endif // TRIDENT_PARSER_H
