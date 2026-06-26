#ifndef TRIDENT_PARSER_I_H
#define TRIDENT_PARSER_I_H

#include <stdbool.h>

#include "parser.h"

typedef enum {
  PREC_UNKNOWN = -1,
  PREC_NONE = 0,
  PREC_COMMA,
  PREC_ASSIGNMENT,
  PREC_RANGE,
  PREC_OR,
  PREC_XOR,
  PREC_AND,
  PREC_BIT_OR,
  PREC_BIT_XOR,
  PREC_BIT_AND,
  PREC_RELATIVE,
  PREC_COMPARITIVE,
  PREC_BITSHIFT,
  PREC_ADDITIVE,
  PREC_MULTIPLICATIVE,
} Precedence;

Precedence get_op_prec(TokenKind kind);
bool is_proc_left_Associative(Precedence prec);

bool is_kind_prefix(TokenKind kind);
bool is_kind_literal(TokenKind kind);

Token *peak_p(Parser *p, int offset);
TokenKind peak_kind(Parser *p, int offset);
Token *consume_p(Parser *p);
void expect_kind(Parser *p, TokenKind kind);

#endif // TRIDENT_PARSER_I_H
