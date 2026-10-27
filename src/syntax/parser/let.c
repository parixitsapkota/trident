#include "core/source.h"
#include "parser.h"

#include "syntax/parser/process.h"
#include "syntax/token/token.h"
#include "syntax/tree/ast.h"

AstNode *parse_let_stmt(Parser *p) {
    Span *start = pconsume(p)->span;
    Token *type = pconsume(p);
    (void)type;

    AstNode *expr = parse_expr(p, PREC_NONE);

    Span *span = span_merge(start, current(p)->span);
    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_stmt(base, AST_LET, expr);
}
