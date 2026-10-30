#include <stdbool.h>
#include <stddef.h>

#include "parser.h"
#include "process.h"

#include "core/source/source.h"
#include "syntax/ast/ast.h"
#include "syntax/token/token.h"

static AstNode *parse_primary(Parser *p);

static bool is_kind_literal(TokenKind kind) {
    switch (kind) {
    case INT_LIT:
    case STRING_LIT:
    case IDENTIFIER_LIT: return true;
    default: return false;
    }
}

static bool is_unary_prefix(TokenKind kind) {
    switch (kind) {
    case PUNCT_NOT:
    case PUNCT_ADD:
    case PUNCT_SUB:
    case PUNCT_BIT_AND: return true;
    default: return false;
    }
}

static bool is_proc_left_Associative(Precedence prec) {
    switch (prec) {
    case PREC_ASSIGNMENT: return true;
    default: return false;
    }
}

static Precedence get_op_prec(TokenKind kind) {
    switch (kind) {
    case PUNCT_ASSIGN: return PREC_ASSIGNMENT;

    case PUNCT_BIT_OR: return PREC_BIT_OR;

    case PUNCT_BIT_AND: return PREC_BIT_AND;

    case PUNCT_LESSER:
    case PUNCT_GREATER:
    case PUNCT_LESSER_EQUAL:
    case PUNCT_GREATER_EQUAL: return PREC_RELATIONAL;

    case PUNCT_EQUAL:
    case PUNCT_NOT_EQUAL: return PREC_EQUALITY;

    case PUNCT_ADD:
    case PUNCT_SUB: return PREC_ADDITIVE;

    case PUNCT_BITSHIFT_L:
    case PUNCT_BITSHIFT_R: return PREC_BITSHIFT;

    case PUNCT_MUL:
    case PUNCT_DEV:
    case PUNCT_MOD: return PREC_MULTIPLICATIVE;

    case PUNCT_COMMA:
    case PUNCT_C_PREN:
    case PUNCT_C_BRACKET:
    case PUNCT_SEMICOLON: return PREC_NONE;

    default: return PREC_UNKNOWN;
    }
}

AstNode *parse_atom(Parser *p) {
    Token *tok = current(p);
    if (is_kind_literal(tok->kind)) tok = pconsume(p);

    AstNode *base = ast_new(p->nodes, tok->span, current(p));

    switch (tok->kind) {
    case INT_LIT: return ast_int_val(base, tok->val.i);
    case IDENTIFIER_LIT: return ast_ident_val(base, tok->lexeme);
    case STRING_LIT: return ast_string_val(base, tok->lexeme);
    default:
        return parser_error(p, NULL, "expected a literal, but got `%s`",
                            token_kind_to_str(tok->kind));
    }
}

AstNode *parse_paren_expr(Parser *p) {
    expect_and_consume(p, PUNCT_O_PREN);

    if (is_expression_delimiter(current(p)->kind)) {
        parser_error(p, NULL, "expected a expression");
        pconsume(p);
        return NULL;
    }

    AstNode *node = parse_expr(p, PREC_NONE);
    expect_and_consume(p, PUNCT_C_PREN);
    return node;
}

static AstNode *parse_subscript(Parser *p, AstNode *target) {
    pconsume(p);
    AstNode *index = parse_expr(p, PREC_NONE);

    Span *span = span_merge(target->span, current(p)->span);
    expect_and_consume(p, PUNCT_C_BRACKET);

    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_index(base, target, index);
}

static AstNode *parse_unary(Parser *p);

static AstNode *parse_postfix(Parser *p) {
    AstNode *expr = parse_primary(p);
    if (!expr) return NULL;

    for (;;) {
        TokenKind kind = current(p)->kind;

        if (kind == PUNCT_O_BRACKET) {
            expr = parse_subscript(p, expr);
        } else if (kind == PUNCT_MUL) {
            Token *op = pconsume(p);
            Span *span = span_merge(expr->span, op->span);
            AstNode *base = ast_new(p->nodes, span, current(p));
            expr = ast_unary(base, op->kind, true, expr);
        } else if (kind == PUNCT_DOT) {
            Token *dot = pconsume(p);
            AstNode *member = parse_unary(p);
            Span *span = span_merge(expr->span, dot->span);
            AstNode *base = ast_new(p->nodes, span, current(p));
            expr = ast_member(base, expr, member);
        } else {
            break;
        }
    }
    return expr;
}

static AstNode *parse_unary(Parser *p) {
    if (is_unary_prefix(current(p)->kind)) {
        Token *tok = pconsume(p);
        Span *op_span = tok->span;
        TokenKind op = tok->kind;

        AstNode *operand = parse_unary(p);
        if (!operand) return NULL;

        Span *span = span_merge(op_span, operand->span);
        AstNode *base = ast_new(p->nodes, span, current(p));
        return ast_unary(base, op, false, operand);
    }
    return parse_postfix(p);
}

AstNode *parse_call(Parser *p) {
    Token *tok = pconsume(p);
    const char *name = tok->lexeme;
    expect_and_consume(p, PUNCT_O_PREN);

    size_t argc = 0;
    AstNode *first_arg = NULL;
    AstNode *last_arg = NULL;

    while (!is_kind(p, PUNCT_C_PREN)) {
        AstNode *expr = parse_expr(p, PREC_NONE);
        ++argc;

        if (!first_arg) {
            first_arg = expr;
        } else {
            last_arg->next = expr;
        }
        last_arg = expr;

        if (is_kind(p, PUNCT_COMMA)) {
            if (next(p)) {
                if (next(p)->kind == PUNCT_C_PREN)
                    parser_error(p, NULL, "expected a expression after `,`");
            }
            pconsume(p);
        } else {
            break;
        }
    }
    Span *span = span_merge(tok->span, current(p)->span);
    expect_and_consume(p, PUNCT_C_PREN);

    AstNode *base = ast_new(p->nodes, span, current(p));
    return ast_call(base, name, first_arg, argc);
}

static AstNode *parse_primary(Parser *p) {
    TokenKind kind = current(p)->kind;

    if (kind == IDENTIFIER_LIT && next(p) && next(p)->kind == PUNCT_O_PREN) {
        return parse_call(p);
    }
    if (kind == PUNCT_O_PREN) {
        return parse_paren_expr(p);
    }
    return parse_atom(p);
}

static AstNode *parse_expr_(Parser *p, Precedence prec) {
    if (current(p)->kind == PUNCT_SEMICOLON) return NULL;

    Span *span_start = current(p)->span;

    AstNode *left = parse_unary(p);
    if (!left) return NULL;

    while (!is_kind(p, END_OF_TOKEN)) {
        Token *tok = current(p);
        TokenKind op = tok->kind;
        Precedence op_prec = get_op_prec(op);

        if (is_expression_delimiter(tok->kind)) break;
        if (op_prec == PREC_UNKNOWN) {
            if (p->expr_err) break;
            const char *got = token_kind_to_str(tok->last->kind);
            parser_error(p, tok->last->span, "expected `;` after `%s`", got);
            p->expr_err = true;
            break;
        }

        if (op_prec == PREC_NONE || op_prec < prec) break;
        pconsume(p);

        bool assignment = is_proc_left_Associative(op_prec);
        Precedence next_prec = assignment ? op_prec + 1 : op_prec;
        AstNode *right = parse_expr_(p, next_prec);
        if (!right) break;

        Span *span_end = current(p)->span;
        Span *span = span_merge(span_start, span_end);

        AstNode *base = ast_new(p->nodes, span, current(p));
        left = ast_binary(base, op, left, right);
    }

    return left;
}

AstNode *parse_expr(Parser *p, Precedence prec) {
    p->expr_err = false;
    AstNode *node = parse_expr_(p, prec);
    p->expr_err = false;
    return node;
}
