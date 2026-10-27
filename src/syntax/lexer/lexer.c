#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "keywords.h"
#include "lexer.h"
#include "process.h"

#include "core/diags.h"
#include "core/mem.h"
#include "core/source.h"
#include "syntax/token/token.h"

typedef struct {
    const char *text;
    size_t len;
    TokenKind kind;
} Punctuator;

static const Punctuator punctuators[] = {
#define LITERAL(tok, name)
#define KEYWORD(tok, name)
#define PUNCTUATION(tok, str) {str, sizeof(str) - 1, PUNCT_##tok},
#define TOKEN(tok, name)
#define INTRINSIC(tok, name)
#include "syntax/token/token.def"
#undef INTRINSIC
#undef TOKEN
#undef PUNCTUATION
#undef KEYWORD
#undef LITERAL
};

static void lex_comment(Lexer *l, Location start, bool lex_all) {
    consume(l, 2);
    while (!is_at_end(l)) {
        if (peek(l, 0) == '\n') {
            if (lex_all) {
                const Location end = location(l->ln, l->cn, l->i);
                char *comment = substr(l->lexeme, l->buffer, start.i, l->i);
                lexer_emit(l, start, end, COMMENT, comment, (NumValue){0});
            }
            return;
        }
        consume(l, 1);
    }
    fatal("%s:%zu:%zu: Unterminated block comment.", l->src->file_path, start.ln,
          start.cn);
}

static void lex_identifier(Lexer *l, Location start) {
    consume_while(l, is_ident_char);
    const size_t length = l->i - start.i;
    char *word = substr(l->lexeme, l->buffer, start.i, l->i);
    const struct Keyword *keyword = get_keyword_kind(word, length);
    const Location end = location(l->ln, l->cn, l->i);
    TokenKind kind = keyword != NULL ? keyword->kind : IDENTIFIER_LIT;
    lexer_emit(l, start, end, kind, word, (NumValue){0});
}

static bool consume_until_quote(Lexer *l, char quote) {
    while (!is_at_end(l) && peek(l, 0) != '\n') {
        const char c = peek(l, 0);
        if (c == quote) {
            return true;
        }
        if (c == '\\') {
            consume(l, 1);
            if (is_at_end(l) || peek(l, 0) == '\n') {
                return false;
            }
        }
        consume(l, 1);
    }
    return false;
}

static void lex_quoted(Lexer *l, Location start) {
    const char quote = peek(l, 0);
    consume(l, 1);
    const size_t body = l->i;

    if (!consume_until_quote(l, quote)) {
        const char *msg = quote == '"' ? "Unterminated string literal."
                                       : "Unterminated character constant.";
        fatal("%s:%zu:%zu: %s", l->src->file_path, start.ln, start.cn, msg);
        return;
    }

    char *text = substr(l->lexeme, l->buffer, body, l->i);
    const size_t length = decode_escapes(l, start, text, l->i - body);
    consume(l, 1);

    if (quote == '"') {
        const Location end = location(l->ln, l->cn, l->i);
        lexer_emit(l, start, end, STRING_LIT, text, (NumValue){0});
        return;
    }

    if (length == 0) lexer_error(l, start, "Empty character constant.");
    if (length > 2) lexer_error(l, start, "Character constant too long.");
    const Location end = location(l->ln, l->cn, l->i);
    lexer_emit(l, start, end, CHARACTER_LIT, text, (NumValue){0});
}

static void lex_quoted_ident(Lexer *l, Location start) {
    consume(l, 1);
    const size_t body = l->i;
    bool closed = false;

    while (true) {
        const char ch = peek(l, 0);
        if (ch == '"') {
            closed = true;
            break;
        }
        if (ch == '\0' || ch == '\n') break;
        if (ch == '\\' && peek(l, 1) != '\0' && peek(l, 1) != '\n') consume(l, 1);
        consume(l, 1);
    }

    char *word = substr(l->lexeme, l->buffer, body, l->i);
    const size_t n = decode_escapes(l, start, word, strlen(word));

    if (closed) {
        consume(l, 1);
        if (n == 0) lexer_error(l, start, "Quoted identifier is empty.");
    } else {
        lexer_error(l, start, "Unterminated quoted identifier.");
    }

    const Location end = location(l->ln, l->cn, l->i);
    lexer_emit(l, start, end, IDENTIFIER_LIT, word, (NumValue){0});
    return;
}

static bool lex_punctuator(Lexer *l, Location start) {
    const size_t count = sizeof(punctuators) / sizeof(punctuators[0]);
    const Punctuator *best = NULL;
    for (size_t k = 0; k < count; ++k) {
        const Punctuator *p = &punctuators[k];
        if (l->i + p->len > l->buf_len ||
            memcmp(l->buffer + l->i, p->text, p->len) != 0) {
            continue;
        }
        if (best == NULL || p->len > best->len) {
            best = p;
        }
    }
    if (best == NULL) {
        return false;
    }
    consume(l, best->len);
    const Location end = location(l->ln, l->cn, l->i);
    lexer_emit(l, start, end, best->kind, NULL, (NumValue){0});
    return true;
}

static void lex_white_space(Lexer *l, Location start) {
    const char c = peek(l, 0);
    TokenKind kind = UNKNOWN_TOKEN;
    if (c == '\n') {
        consume(l, 1);
        kind = NEW_LINE;
    } else {
        while (is_space(peek(l, 0))) {
            consume(l, 1);
        }
        kind = WHITE_SPACE;
    }
    const Location end = location(l->ln, l->cn, l->i);
    lexer_emit(l, start, end, kind, NULL, (NumValue){0});
}

Lexer *init_lexer(Source *src) {
    Lexer *l = xmalloc(sizeof(Lexer));
    *l = (Lexer){0};
    l->src = src;
    l->buffer = src->buffer;
    l->buf_len = src->buffer_len;
    l->ln = 1;
    l->cn = 1;

    l->lexeme = init_arena(src->buffer_len * 2 + 1);
    l->token = init_arena(sizeof(Token) * 1024);
    l->spans = init_arena(sizeof(Span) * 1024);

    l->token_head = arena_alloc(l->token, sizeof(Token));
    l->token_head->next = NULL;
    l->token_tail = l->token_head;

    return l;
}

Token *lex(Lexer *l, bool lex_all) {
    while (!is_at_end(l)) {
        const Location start = location(l->ln, l->cn, l->i);
        const char c = peek(l, 0);
        l->t_cn = start.cn;

        if (is_space(c)) {
            if (lex_all) {
                lex_white_space(l, start);
            } else {
                consume(l, 1);
            }
            continue;
        }
        if (c == '/' && peek(l, 1) == '/') {
            lex_comment(l, start, lex_all);
            continue;
        }
        if (is_alpha(c) || c == '_') {
            lex_identifier(l, start);
            continue;
        }
        if (starts_number(l)) {
            lex_number(l, start);
            continue;
        }
        if (c == '"' || c == '\'') {
            lex_quoted(l, start);
            continue;
        }
        if (c == '@') {
            consume(l, 1);
            if (peek(l, 0) == '"') {
                lex_quoted_ident(l, start);
            } else if (is_alpha(peek(l, 0))) {
                lex_intrinsic(l, start);
            } else {
                lexer_error(l, start, "lonely `@'. expected a ident or a intrinsic.");
            }
            continue;
        }
        if (lex_punctuator(l, start)) {
            continue;
        }

        lexer_error(l, start, "Unexpected character '%c'.", c);
        consume(l, 1);
    }
    // add the last token.
    Location loc_end = location(l->ln, l->cn, l->i);
    lexer_emit(l, loc_end, loc_end, END_OF_TOKEN, "EOT", (NumValue){0});
    // nulling the first token's last token ptr.
    l->token_head->next->last = NULL;

    if (l->errorc > 0) fatal("Lexer had %zu error(s).", l->errorc);

    return l->token_head->next;
}

void free_lexer(Lexer *l) {
    free_arena(l->lexeme);
    free_arena(l->token);
    free_arena(l->spans);
    free(l);
}
