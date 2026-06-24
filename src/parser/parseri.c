#include "parseri.h"
#include "error/error.h"
#include "shi_opa.h"

Token *peak_p(Parser *p, int offset) { return (Token *)shi_opa_index(p->tokens_pool, p->index + offset); }

TokenKind peak_kind(Parser *p, int offset) { return peak_p(p, 0)->kind; }

Token *consume_p(Parser *p) {
  ++p->index;
  peak_p(p, 0);
}

void expect_kind(Parser *p, Error *e, TokenKind kind) {
  Token *tok = consume_p(p);
  if (tok->kind != kind) {
    // TODO: add a error to errors.
    return;
  }
}
