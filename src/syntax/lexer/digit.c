#include <stdlib.h>

#include "process.h"

#include "core/mem.h"

static size_t parse_int(const char *text) {
    if (text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) {
        return (size_t)strtoull(text + 2, NULL, 2);
    }
    return (size_t)strtoull(text, NULL, 0);
}

static void lex_radix_digits(Lexer *l, Location start, bool (*is_valid)(char),
                             const char *empty_msg) {
    consume(l, 2);
    if (consume_while(l, is_valid) == 0) lexer_error(l, start, "%s", empty_msg);
}

static bool lex_fraction_exponent(Lexer *l) {
    bool is_float = false;

    if (peek(l, 0) == '.' && peek(l, 1) != '.') {
        is_float = true;
        consume(l, 1);
        consume_while(l, is_digit);
    }

    const char e = peek(l, 0);
    if (e == 'e' || e == 'E') {
        const char sign = peek(l, 1);
        const int off = (sign == '+' || sign == '-') ? 2 : 1;
        if (is_digit(peek(l, off))) {
            is_float = true;
            consume(l, off);
            consume_while(l, is_digit);
        }
    }

    return is_float;
}

void lex_number(Lexer *l, Location start) {
    const char c0 = peek(l, 0);
    const char next = peek(l, 1);
    bool is_float = false;
    bool is_radix = false;

    if (c0 == '0' && (next == 'x' || next == 'X')) {
        is_radix = true;
        lex_radix_digits(l, start, is_xdigit, "Hex constant has no digits.");
    } else if (c0 == '0' && (next == 'b' || next == 'B')) {
        is_radix = true;
        lex_radix_digits(l, start, is_bin_digit, "Binary constant has no digits.");
    } else {
        consume_while(l, is_digit);
        is_float = lex_fraction_exponent(l);
        if (is_float) {
            const char s = peek(l, 0);
            if (s == 'f' || s == 'F' || s == 'l' || s == 'L') consume(l, 1);
        }
    }

    if (is_ident_char(peek(l, 0))) {
        lexer_error(l, start, "Invalid character `%c` in numeric constant.", peek(l, 0));
        consume_while(l, is_ident_char);
    }

    char *text = substr(l->lexeme, l->buffer, start.i, l->i);
    const Location end = location(l->ln, l->cn, l->i);

    if (is_float) {
        NumValue val;
        val.f = strtod(text, NULL);
        lexer_emit(l, start, end, FLOAT_LIT, text, val);
        return;
    }

    if (!is_radix && text[0] == '0' && text[1]) {
        for (const char *p = text + 1; *p; p++) {
            if (!is_octal(*p)) {
                lexer_error(l, start, "Invalid digit '%c' in octal constant.", *p);
                break;
            }
        }
    }

    NumValue val;
    val.i = parse_int(text);
    lexer_emit(l, start, end, INT_LIT, text, val);
}
