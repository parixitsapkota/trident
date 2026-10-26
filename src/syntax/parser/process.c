#include <stdarg.h>
#include <stdbool.h>

#include "process.h"

#include "core/diags.h"
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

bool expect(Parser *p, TokenKind kind) {
    Token *tok = current(p);

    if (!tok || tok->kind != kind) {
        ++p->errorc;
        const char *got =
            tok ? (tok->lexeme ? tok->lexeme : token_kind_to_str(tok->kind)) : "EOF";
        diag_err(p->src, tok ? tok->span : NULL, "expected `%s`, but found `%s`",
                 token_kind_to_str(kind), got);
        return false;
    }

    return true;
}

bool expect_and_consume(Parser *p, TokenKind kind) {
    if (!expect(p, kind)) {
        return false;
    }
    pconsume(p);
    return true;
}

inline bool is_kind(Parser *p, TokenKind kind) { return current(p)->kind == kind; }

void *parser_error(Parser *p, const char *fmt, ...) {
    ++p->errorc;
    va_list args;
    va_start(args, fmt);
    vdiag_err(p->src, current(p)->span, false, fmt, args);
    va_end(args);
    return NULL;
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
