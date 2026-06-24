#ifndef TRIDENT_PARSER_I_H
#define TRIDENT_PARSER_I_H

#include "parser.h"

Token *peak_p(Parser *p, int offset);
TokenKind peak_kind(Parser *p, int offset);
void consume_p(Parser *p);
void expect_kind(Parser *p, TokenKind kind);

#endif // TRIDENT_PARSER_I_H
