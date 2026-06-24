#include "parser.h"
#include "ast.h"
#include "parseri.h"
#include "shi_opa.h"

#define AST_CAP 1024

Parser *init_parser(Lexer *l) {
  static Parser p = {0};
  p.ast_pool_head = shi_opa_init(AstNode, AST_CAP);
  p.ast_pool = p.ast_pool_head;
  p.tokens_pool = l->token_pool_head;
  return &p;
}

void parse(Parser *p) { return; }

void free_parser(Parser *p) { shi_opa_free(p->ast_pool_head); }
