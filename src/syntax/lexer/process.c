#include "process.h"

#include "core/diags.h"

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
