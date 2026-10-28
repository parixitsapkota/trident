#include "ast.h"

#include "core/mem.h"
#include "syntax/token/token.h"

AstNode *ast_new(Arena *arena, Span *span, Token *trivia) {
    AstNode *base = arena_alloc(arena, sizeof(AstNode));
    *base = (AstNode){.span = span, .trivia = trivia};
    return base;
}

AstNode *ast_ident_val(AstNode *base, const char *name) {
    base->kind = AST_IDENT;
    base->as.ident.name = name;
    return base;
}

AstNode *ast_int_val(AstNode *base, size_t int_val) {
    base->kind = AST_INT;
    base->as.int_val = int_val;
    return base;
}

AstNode *ast_float_val(AstNode *base, float float_val) {
    base->kind = AST_FLOAT;
    base->as.float_val = float_val;
    return base;
}

AstNode *ast_char_val(AstNode *base, char char_val) {
    base->kind = AST_CHAR;
    base->as.char_val = char_val;
    return base;
}

AstNode *ast_string_val(AstNode *base, const char *str) {
    base->kind = AST_STRING;
    base->as.str = str;
    return base;
}

AstNode *ast_unary(AstNode *base, TokenKind op, bool postfix, AstNode *operand) {
    base->kind = AST_UNARY;
    base->as.unary.op = op;
    base->as.unary.postfix = postfix;
    base->as.unary.operand = operand;
    return base;
}

AstNode *ast_binary(AstNode *base, TokenKind op, AstNode *left, AstNode *right) {
    base->kind = AST_BINARY;
    base->as.binary.op = op;
    base->as.binary.lhs = left;
    base->as.binary.rhs = right;
    return base;
}

AstNode *ast_index(AstNode *base, AstNode *left, AstNode *index) {
    base->kind = AST_INDEX;
    base->as.index.left = left;
    base->as.index.index = index;
    return base;
}

AstNode *ast_call(AstNode *base, const char *callee, AstNode *args, size_t argc) {
    base->kind = AST_CALL;
    base->as.call.callee = callee;
    base->as.call.args = args;
    base->as.call.argc = argc;
    return base;
}

AstNode *ast_stmt(AstNode *base, AstKind kind, AstNode *expr) {
    base->kind = kind;
    base->as.stmt.expr = expr;
    return base;
}

AstNode *ast_let(AstNode *base, AstNode *expr, Token *type) {
    base->kind = AST_LET;
    base->as.let.expr = expr;
    base->as.let.type = type;
    return base;
}

AstNode *ast_cond(AstNode *base, AstKind kind, AstNode *cond, AstNode *then_b,
                  AstNode *else_b) {
    base->kind = kind;
    base->as.cond.cond = cond;
    base->as.cond.then_b = then_b;
    base->as.cond.else_b = else_b;
    return base;
}

AstNode *ast_ctl(AstNode *base, AstKind kind, AstNode *cond, AstNode *body) {
    base->kind = kind;
    base->as.ctl.expr = cond;
    base->as.ctl.body = body;
    return base;
}

AstNode *ast_block(AstNode *base, AstNode *stmts) {
    base->kind = AST_BLOCK;
    base->as.block.stmts = stmts;
    return base;
}

AstNode *ast_function(AstNode *base, bool noreturn, bool public, const char *name,
                      AstNode *params, size_t nparams, AstNode *body,
                      Token *return_type) {
    base->kind = AST_FUNCTION;
    base->as.func.noreturn = noreturn;
    base->as.func.public = public;
    base->as.func.name = name;
    base->as.func.params = params;
    base->as.func.nparams = nparams;
    base->as.func.body = body;
    base->as.func.return_type = return_type;
    return base;
}

AstNode *ast_param(AstNode *base, const char *name, Token *type) {
    base->kind = AST_PARAM;
    base->as.param.name = name;
    base->as.param.type = type;
    return base;
}
