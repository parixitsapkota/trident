#include <stdio.h>
#include <string.h>

#include "intrinsics.h"
#include "process.h"

#include "core/diags.h"
#include "core/mem.h"
#include "syntax/token/token.h"

char peek(const Lexer *l, size_t offset) {
    if (l->i + offset >= l->buf_len) {
        return '\0';
    }
    return l->buffer[l->i + offset];
}

bool starts_number(Lexer *l) {
    const char c = peek(l, 0);
    return is_digit(c) || (c == '.' && is_digit(peek(l, 1)));
}

void consume(Lexer *l, size_t count) {
    for (size_t k = 0; k < count && l->i < l->buf_len; ++k) {
        if (l->buffer[l->i] == '\n') {
            ++l->ln;
            l->cn = 1;
        } else {
            ++l->cn;
        }
        ++l->i;
    }
}

size_t consume_while(Lexer *l, bool (*pred)(char)) {
    size_t count = 0;
    while (!is_at_end(l) && pred(peek(l, 0))) {
        consume(l, 1);
        ++count;
    }
    return count;
}

void lexer_error(Lexer *l, Location start, const char *fmt, ...) {
    ++l->errorc;
    fprintf(stderr, "%s:%zu:%zu: ", l->src->file_path, start.ln, start.cn);
    va_list args;
    va_start(args, fmt);
    verror(fmt, args);
    va_end(args);
}

void lexer_emit(Lexer *l, Location start, Location end, TokenKind kind,
                const char *lexeme, NumValue val) {
    Span *s = span(l->spans, start, end);
    Token *new_token = token(l->token, kind, lexeme, s, val);
    Token *last_token = l->token_tail;
    new_token->last = last_token;
    l->token_tail->next = new_token;
    l->token_tail = new_token;
}

void lex_intrinsic(Lexer *l, Location start) {
    const size_t start_i = l->i;
    consume_while(l, is_ident_char);
    const size_t length = l->i - start_i;
    char *word = substr(l->lexeme, l->buffer, start_i, l->i);
    const struct Intrinsic *intrinsic = get_intrinsic_kind(word, length);

    const Location end = location(l->ln, l->cn, l->i);

    if (intrinsic == NULL) {
        lexer_error(l, start, "unknown intrensic `@%s'.", word);
        lexer_emit(l, start, end, UNKNOWN_TOKEN, word, (NumValue){0});
        return;
    }
    TokenKind kind = intrinsic->kind;

    if (kind == IC_HERE) {
        const char *file_path = l->src->file_path;
        word = arena_alloc(l->lexeme, strlen(file_path) + 22);
        sprintf(word, "%s:%zu:%zu", file_path, start.ln, start.cn);
        kind = STRING_LIT;
    }

    lexer_emit(l, start, end, kind, word, (NumValue){0});
}
