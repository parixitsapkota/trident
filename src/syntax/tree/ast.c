#include "ast.h"
#include "core/arena.h"

VarInfo *var_info(Arena *arena, VarKind kind, Type *type) {
    VarInfo *info = arena_alloc(arena, sizeof(VarInfo));
    *info = (VarInfo){kind, type};
    return info;
}

AstNode *ast_new(Arena *arena, AstKind kind, Span *span) {
    AstNode *node = arena_alloc(arena, sizeof(AstNode));
    *node = (AstNode){.kind = kind, .span = span};
    return node;
}

AstNode *ast_ident_val(Arena *a, Span *span, const char *name) {
    AstNode *node = ast_new(a, AST_IDENT, span);
    node->as.ident.name = name;
    return node;
}

AstNode *ast_int_val(Arena *a, Span *span, size_t int_val) {
    AstNode *node = ast_new(a, AST_INT, span);
    node->as.int_val = int_val;
    return node;
}

AstNode *ast_float_val(Arena *a, Span *span, float float_val) {
    AstNode *node = ast_new(a, AST_FLOAT, span);
    node->as.float_val = float_val;
    return node;
}

AstNode *ast_char_val(Arena *a, Span *span, char char_val) {
    AstNode *node = ast_new(a, AST_CHAR, span);
    node->as.char_val = char_val;
    return node;
}

AstNode *ast_string_val(Arena *a, Span *span, const char *str) {
    AstNode *node = ast_new(a, AST_STRING, span);
    node->as.str = str;
    return node;
}

AstNode *ast_unary(Arena *a, Span *span, TokenKind op, bool postfix, AstNode *operand) {
    AstNode *node = ast_new(a, AST_UNARY, span);
    node->as.unary.op = op;
    node->as.unary.postfix = postfix;
    node->as.unary.operand = operand;
    return node;
}

AstNode *ast_binary(Arena *a, Span *span, AstKind kind, TokenKind op, AstNode *left,
                    AstNode *right) {
    AstNode *node = ast_new(a, kind, span);
    node->as.binary.op = op;
    node->as.binary.lhs = left;
    node->as.binary.rhs = right;
    return node;
}

AstNode *ast_index(Arena *a, Span *span, AstNode *left, AstNode *index) {
    AstNode *node = ast_new(a, AST_INDEX, span);
    node->as.index.left = left;
    node->as.index.index = index;
    return node;
}

AstNode *ast_call(Arena *a, Span *span, const char *callee, AstNode *args, size_t argc) {
    AstNode *node = ast_new(a, AST_INDEX, span);
    node->as.call.callee = callee;
    node->as.call.args = args;
    node->as.call.argc = argc;
    return node;
}
