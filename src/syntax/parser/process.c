#include <stdarg.h>
#include <stdbool.h>

#include "process.h"

#include "core/diags/diags.h"
#include "syntax/token/token.h"

inline Token *current(Parser *p) { return p->tok_tail; }

inline Token *next(Parser *p) { return p->tok_tail->next; }

Token *pconsume(Parser *p) {
    Token *c = current(p);
    if (next(p))
        p->tok_tail = p->tok_tail->next;
    else
        p->tok_tail = NULL;
    return c;
}

Token *expect_and_consume(Parser *p, TokenKind kind) {
    if (!is_kind(p, kind)) {
        return parser_error(p, NULL, "expected a `%s` before `%s`",
                            token_kind_to_str(kind), token_kind_to_str(current(p)->kind));
    }
    return pconsume(p);
}

inline bool is_kind(Parser *p, TokenKind kind) { return current(p)->kind == kind; }

void *parser_error(Parser *p, Span *span, const char *fmt, ...) {
    ++p->errorc;
    va_list args;
    va_start(args, fmt);
    vdiag_err(p->src, span == NULL ? current(p)->span : span, false, fmt, args);
    va_end(args);
    return NULL;
}

bool is_expression_delimiter(TokenKind kind) {
    switch (kind) {
    case PUNCT_COMMA:
    case PUNCT_C_PREN:
    case PUNCT_C_BRACKET:
    case PUNCT_C_BRACE:
    case PUNCT_COLON:
    case PUNCT_SEMICOLON: return true;

    default: return false;
    }
}

void chain_append(NodeChain *c, AstNode *head) {
    if (!head) return;
    if (c->first) {
        c->last->next = head;
    } else {
        c->first = head;
    }
    c->last = head;
    while (c->last->next)
        c->last = c->last->next;
}
