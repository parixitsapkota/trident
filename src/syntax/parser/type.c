#include "type/type.h"
#include "parser.h"

#include "syntax/ast/ast.h"
#include "syntax/parser/process.h"
#include "syntax/token/token.h"

bool parse_constness(Parser *p) {
    if (is_kind(p, KW_MUT)) {
        pconsume(p);
        return true;
    }
    if (is_kind(p, KW_CONST)) {
        pconsume(p);
        return false;
    }
    return false;
}

// TODO: Use this function globally.
static AstNode *type_base(Parser *p, Token *first) {
    Span *span = span_merge(first->span, current(p)->last->span);
    return ast_new(p->nodes, span, first);
}

AstNode *parse_type(Parser *p) {
    Token *t = current(p);

    if (is_kind(p, PUNCT_MUL)) {
        pconsume(p);
        bool mut = parse_constness(p);
        AstNode *inner = parse_type(p);
        if (!inner) return NULL;
        return ast_type_pointer(type_base(p, t), AST_TYPE_POINTER, inner, mut);
    }

    if (is_kind(p, PUNCT_O_BRACKET)) {
        pconsume(p);

        if (is_kind(p, PUNCT_C_BRACKET)) { // slice
            pconsume(p);
            bool mut = parse_constness(p);
            AstNode *inner = parse_type(p);
            if (!inner) return NULL;
            return ast_type_pointer(type_base(p, t), AST_TYPE_SLICE, inner, mut);
        }

        AstNode *len = parse_expr(p, PREC_NONE); // array
        if (!len) return NULL;
        if (!expect_and_consume(p, PUNCT_C_BRACKET)) return NULL;

        if (is_kind(p, KW_MUT) || is_kind(p, KW_CONST)) {
            parser_error(p, NULL,
                         "arrays take mutability from the binding, not the type");
            pconsume(p);
        }
        AstNode *inner = parse_type(p);
        if (!inner) return NULL;
        return ast_type_array(type_base(p, t), inner, len);
    }

    if (is_kind(p, KW_FUNC)) {
        pconsume(p);
        expect_and_consume(p, PUNCT_O_PREN);
        NodeChain args_types = {0};
        while (!is_kind(p, PUNCT_C_PREN)) {
            chain_append(&args_types, parse_type(p));
            if (is_kind(p, PUNCT_COMMA)) pconsume(p);
        }
        expect_and_consume(p, PUNCT_C_PREN);
        expect_and_consume(p, PUNCT_ARROW);
        AstNode *return_type = parse_type(p);
        return ast_type_func(type_base(p, t), args_types.first, return_type);
    }

    if (is_primitive_kind(t->kind) || is_kind(p, IDENTIFIER_LIT)) {
        pconsume(p);
        return ast_type(type_base(p, t), t);
    }

    parser_error(p, NULL, "expected a type, found `%s'", token_kind_to_str(t->kind));
    return NULL;
}
