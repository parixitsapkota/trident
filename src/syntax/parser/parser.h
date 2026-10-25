#pragma once

#include "core/source.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

typedef struct Parser Parser;

Parser *init_parser(Source *src, Token *tokens);
AstNode *parse(Parser *p);
void free_Parser(Parser *p);
