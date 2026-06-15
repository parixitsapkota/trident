#include <stddef.h>
#include <stdio.h>

#include "../../SHI/shi_opa.h"
#include "parser.h"

Parser init_parser(const SHI_OPA *token_pool, const SHI_OPA *ast_pool) {
  Parser p = {0};
  p.token_pool = (SHI_OPA *)token_pool;
  p.token_pool = (SHI_OPA *)ast_pool;
  return p;
}

SHI_OPA *parser(const SHI_OPA *token_pool) {
  const SHI_OPA *ast_pool_head = shi_opa_init(AstNode, 1024);
  Parser p = init_parser(token_pool, ast_pool_head);

  Token *token = shi_opa_index(token_pool, 0);
  for (size_t index = 0; token->kind != END_OF_TOKEN; ++index) {
    token = shi_opa_index(token_pool, index);
    parse_expr(&p);
  }

  return (SHI_OPA *)ast_pool_head;
}
