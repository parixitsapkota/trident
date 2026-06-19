#include "parser.h"
#include "../../SHI/shi_opa.h"

#include <stddef.h>
#include <stdio.h>

Parser init_parser(const SHI_OPA *token_pool, const SHI_OPA *ast_pool) {
  Parser p = {0};
  p.token_pool = (SHI_OPA *)token_pool;
  p.ast_pool = (SHI_OPA *)ast_pool;
  p.ast_pool_head = (SHI_OPA *)ast_pool;
  return p;
}

SHI_OPA *parser(const LexReturn *l) {
  Parser p = init_parser(l->token_pool, shi_opa_init(AstNode, 1024));

  Token *token = shi_opa_index(l->token_pool, 0);
  for (size_t index = 0; token->kind != END_OF_TOKEN; ++index) {
    token = shi_opa_index(l->token_pool, index);
    parse_expr(&p);
  }

  return p.ast_pool_head;
}

Token *parser_current(Parser *self) {
  return (Token *)shi_opa_index(self->token_pool, self->index);
}

TokenKind parser_current_kind(Parser *self) {
  return parser_current(self)->kind;
}

Token *parser_peak(Parser *self, int offset) {
  return (Token *)shi_opa_index(self->token_pool, self->index + offset);
}

TokenKind parser_peak_kind(Parser *self, int offset) {
  return parser_peak(self, offset)->kind;
}

void parser_consume(Parser *self) { ++self->index; }

void parser_Expect(Parser *self, TokenKind kind) {
  TokenKind got = parser_current_kind(self);
  if (got != kind)
    printf("Expected : %d but got %d\n", kind, got);
  else
    parser_consume(self);
}
