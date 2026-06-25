#include "parseri.h"
#include "error/error.h"
#include "shi_opa.h"

Precedence get_op_prec(TokenKind kind) {
  switch (kind) {

  case STAR:
  case DEV:
  case MOD: return PREC_MULTIPLICATIVE;

  case ADD:
  case SUB: return PREC_ADDITIVE;

  case SHIFT_LEFT:
  case SHIFT_RIGHT: return PREC_BITSHIFT;

  case GREATER:
  case LESSER:
  case GREATER_EQUAL:
  case LESSER_EQUAL: return PREC_COMPARITIVE;

  case EQUAL:
  case NOT_EQUAL: return PREC_RELATIVE;

  case ANDPERCENT: return PREC_BIT_AND;
  case BIT_XOR: return PREC_BIT_XOR;
  case BIT_OR: return PREC_BIT_OR;

  case LOGICAL_AND: return PREC_AND;
  case LOGICAL_XOR: return PREC_XOR;
  case LOGICAL_OR: return PREC_OR;

  case INCLUSIVE_RANGE: return PREC_RANGE;

  case ASSIGN:
  case ADD_ASSIGN:
  case SUB_ASSIGN:
  case MUL_ASSIGN:
  case DEV_ASSIGN:
  case MOD_ASSIGN:
  case SHIFT_LEFT_ASSIGN:
  case SHIFT_RIGHT_ASSIGN:
  case BIT_AND_ASSIGN:
  case BIT_OR_ASSIGN:
  case BIT_XOR_ASSIGN: return PREC_ASSIGNMENT;

  case COMMA: return PREC_COMMA;

  case SEMICOLON: return PREC_NONE;

  default: return PREC_UNKNOWN;
  }
}

bool is_proc_left_Associative(Precedence prec) {
  switch (prec) {
  case PREC_ASSIGNMENT: return false;
  default: return true;
  }
}

Token *peak_p(Parser *p, int offset) { return (Token *)shi_opa_index(p->tokens_pool, p->index + offset); }

TokenKind peak_kind(Parser *p, int offset) { return peak_p(p, offset)->kind; }

Token *consume_p(Parser *p) {
  Token *tok = peak_p(p, 0);
  ++p->index;
  return tok;
}

void expect_kind(Parser *p, TokenKind kind) {
  Token *tok = consume_p(p);
  if (tok->kind != kind) {
    // TODO: add a error to errors.
    return;
  }
}
