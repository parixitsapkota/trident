#include "parser.h"

bool is_kind_prifix(TokenKind kind) {
  switch (kind) {
  case DASH:
  case NEGATE:
  case STAR:
  case INCREMENT:
  case DECREMENT:
    return true;
  default:
    return false;
  }
}

bool is_kind_postfix(TokenKind kind) {
  switch (kind) {
  case INCREMENT:
  case DECREMENT:
    return true;
  default:
    return false;
  }
}

bool is_kind_literal(TokenKind kind) {
  switch (kind) {
  case IDENTIFIER:
  case INT:
  case FLOAT:
  case STRING:
    return true;
  default:
    return false;
  }
}

void parse_atom(Parser *p) { (void)p; }

void parse_expr(Parser *p) { parse_atom(p); }
