#include "cst.h"
#include "core/arena.h"

CstNode *new_cst_node(Arena *arena, CstKind kind, Span span) {
    CstNode *node = arena_alloc(arena, sizeof(CstNode));
    *node = (CstNode){.kind = kind, .span = span};
    return node;
}

CstNode *new_cst_leaf(Arena *arena, Token *token, Span span) {
    CstNode *node = new_cst_node(arena, CST_LEAF, span);
    node->as.leaf.tok = token;
    return node;
}

CstNode *new_cst_unary(Arena *arena, CstNode *operand, Token *op, Span span) {
    CstNode *node = new_cst_node(arena, CST_LEAF, span);
    node->as.unary.operand = operand;
    node->as.unary.op = op;
    return node;
}

CstNode *new_cst_binary(Arena *arena, CstNode *lhs, Token *op, CstNode *rhs, Span span) {
    CstNode *node = new_cst_node(arena, CST_LEAF, span);
    node->as.binary.lhs = lhs;
    node->as.binary.op = op;
    node->as.binary.rhs = rhs;
    return node;
}
