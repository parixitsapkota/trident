#pragma once

#include "core/source/source.h"
#include "syntax/ast/ast.h"
#include "syntax/token/token.h"

typedef struct Parser Parser;

Parser *init_parser(Source *src, Token *tokens);
AstNode *parse(Parser *p);
void free_Parser(Parser *p);
