#include "ast.h"
#include "core/mem.h"

VarInfo *var_info(Arena *arena, VarKind kind, Type *type) {
    VarInfo *info = arena_alloc(arena, sizeof(VarInfo));
    *info = (VarInfo){kind, type};
    return info;
}

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
