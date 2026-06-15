#include <stddef.h>

#include "parser.h"

void parse_expr(const SHI_OPA *token_pool, size_t *index, SHI_OPA *ast) {
  (void)token_pool;
  (void)index;
  (void)ast;
}

SHI_OPA *parser(const SHI_OPA *token_pool) {
  const SHI_OPA *ast_pool_head = NULL;
  SHI_OPA *ast_pool = (SHI_OPA *)ast_pool_head;

  Token *token = shi_opa_index(token_pool, 0);
  for (size_t index = 0; token->kind != END_OF_TOKEN; ++index) {
    token = shi_opa_index(token_pool, index);

    parse_expr(token_pool, &index, NULL);
  }

  (void)ast_pool;
  return (SHI_OPA *)ast_pool_head;
}
