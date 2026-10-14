#include "lexer.h"

#include <stdbool.h>
#include <stddef.h>

#include "process.h"

static int digit_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool is_bin_char(char c) { return c == '0' || c == '1'; }

static int simple_escape(char c) {
    switch (c) {
    case 'a': return '\a';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    case 'e': return 27;
    case '\\': return '\\';
    case '\'': return '\'';
    case '"': return '"';
    case '?': return '?';
    default: return -1;
    }
}

static size_t encode_utf8(char *out, unsigned long cp) {
    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

size_t decode_escapes(Lexer *l, Location start, char *s, size_t len) {
    size_t r = 0, w = 0;
    while (r < len) {
        if (s[r] != '\\') {
            s[w++] = s[r++];
            continue;
        }
        ++r;
        if (r >= len) break;

        const char c = s[r];
        const int simple = simple_escape(c);
        if (simple >= 0) {
            s[w++] = (char)simple;
            ++r;
            continue;
        }

        unsigned base = 0;
        size_t first = 0, max_digits = 0;
        bool exact = false, force_utf8 = false;

        switch (c) {
        case 'x':
            base = 16;
            first = r + 1;
            max_digits = SIZE_MAX;
            break;
        case 'u':
            base = 16;
            first = r + 1;
            max_digits = 4;
            exact = force_utf8 = true;
            break;
        case 'U':
            base = 16;
            first = r + 1;
            max_digits = 8;
            exact = force_utf8 = true;
            break;
        case 'b':
            if (r + 1 < len && is_bin_char(s[r + 1])) {
                base = 2;
                first = r + 1;
                max_digits = SIZE_MAX;
            } else {
                s[w++] = '\b';
                ++r;
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            if (c == '0' && r + 1 < len && (s[r + 1] == 'x' || s[r + 1] == 'X')) {
                base = 16;
                first = r + 2;
                max_digits = SIZE_MAX;
            } else if (c == '0' && r + 2 < len && (s[r + 1] == 'b' || s[r + 1] == 'B') &&
                       is_bin_char(s[r + 2])) {
                base = 2;
                first = r + 2;
                max_digits = SIZE_MAX;
            } else {
                base = 8;
                first = r;
                max_digits = 3;
            }
            break;
        default:
            lexer_error(l, start, "Unknown escape sequence `\\%c`", c);
            ++r;
            break;
        }

        if (base == 0) continue;

        unsigned long val = 0;
        size_t i = first, count = 0;
        while (i < len && count < max_digits) {
            const int d = digit_value(s[i]);
            if (d < 0 || (unsigned)d >= base) break;
            val = val * base + (unsigned long)d;
            if (val > 0x10FFFF) val = 0x110000;
            ++i;
            ++count;
        }

        if (count == 0 || (exact && count != max_digits)) {
            lexer_error(l, start, "Incomplete escape sequence `\\%c`", c);
        } else if (val > 0x10FFFF) {
            lexer_error(l, start, "Escape sequence `\\%c` is out of range", c);
        } else if (force_utf8 || val > 0xFF) {
            w += encode_utf8(s + w, val);
        } else {
            s[w++] = (char)val;
        }

        r = i;
    }
    s[w] = '\0';
    return w;
}
