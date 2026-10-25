#pragma once

#include "parser.h"

#include "syntax/tree/ast.h"

typedef enum {
    PREC_UNKNOWN = -1,
    PREC_NONE = 0,
    PREC_ASSIGNMENT,
    PREC_CONDITIONAL,
    PREC_BIT_OR,
    PREC_BIT_AND,
    PREC_RELATIONAL,
    PREC_EQUALITY,
    PREC_BITSHIFT,
    PREC_ADDITIVE,
    PREC_MULTIPLICATIVE,
} Precedence;

AstNode *parse_expr(Parser *p, Precedence prec);
