#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "diags.h"

#include "def.h"

#define TAB_WIDTH 4
#define MAX_MID_LINES 4

static void put_n(char c, size_t n) {
    while (n--)
        fputc(c, stderr);
}

static int digits(size_t n) {
    int d = 1;
    while (n >= 10) {
        n /= 10;
        ++d;
    }
    return d;
}

static size_t vcol(const char *s, size_t cn) {
    size_t col = 0, i = 0;
    for (; i < cn && s[i]; ++i)
        col += (s[i] == '\t') ? TAB_WIDTH - col % TAB_WIDTH : 1;
    return col + (cn - i);
}

static void put_expanded(const char *s) {
    size_t col = 0;
    for (; *s; ++s) {
        if (*s == '\t') {
            size_t n = TAB_WIDTH - col % TAB_WIDTH;
            put_n(' ', n);
            col += n;
        } else {
            fputc(*s, stderr);
            ++col;
        }
    }
    fputc('\n', stderr);
}

static void gutter(int w, size_t ln) {
    if (ln)
        fprintf(stderr, DIM " %*zu | " RESET, w, ln);
    else
        fprintf(stderr, DIM " %*s | " RESET, w, "");
}

static void render_mid(int w, const char *col, size_t ln, const char *text) {
    gutter(w, ln);
    fprintf(stderr, "%s|" RESET " ", col);
    put_expanded(text);
}

static void diag_render_span(const Source *src, const Span *sp, bool rh) {
    size_t sl = sp->start.ln;
    size_t el = sp->end.ln < sl ? sl : sp->end.ln;

    if (sl == 0 || el > src->nlines)
        fatal("incorrect line range: %zu-%zu for src: %s", sl, el, src->file_path);

    const char *col = rh ? FGB_RED : FGB_GREEN;
    const char *ls = src->lines[sl - 1];
    int w = digits(el);
    size_t sc = vcol(ls, sp->start.cn);

    if (el == sl) {
        size_t ec = vcol(ls, sp->end.cn);
        size_t tildes = ec > sc + 1 ? ec - sc - 1 : 0;
        --sc;

        gutter(w, sl);
        put_expanded(ls);
        gutter(w, 0);
        put_n(' ', sc);
        fputs(col, stderr);
        fputc('^', stderr);
        put_n('~', tildes);
        fputs(RESET "\n", stderr);
        return;
    }

    const char *le = src->lines[el - 1];
    size_t last = sp->end.cn ? sp->end.cn - 1 : 0;
    size_t ec = vcol(le, last);

    gutter(w, sl);
    fputs("  ", stderr);
    put_expanded(ls);

    gutter(w, 0);
    fputc(' ', stderr);
    fputs(col, stderr);
    put_n('_', sc);
    fputs("^" RESET "\n", stderr);

    size_t ms = sl + 1, me = el - 1;
    if (me >= ms) {
        if (me - ms + 1 > MAX_MID_LINES) {
            render_mid(w, col, ms, src->lines[ms - 1]);
            render_mid(w, col, ms + 1, src->lines[ms]);
            fprintf(stderr, " %*s\n", w < 3 ? 3 : w, "...");
            render_mid(w, col, me, src->lines[me - 1]);
        } else {
            for (size_t l = ms; l <= me; ++l)
                render_mid(w, col, l, src->lines[l - 1]);
        }
    }

    render_mid(w, col, el, le);

    gutter(w, 0);
    fputs(col, stderr);
    fputc('|', stderr);
    put_n('_', ec);
    fputs("^" RESET "\n", stderr);
}

void verror(const char *fmt, va_list args) {
    fputs(BOLD FGB_RED "ERROR: " FGB_WHITE, stderr);
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
}

void vdiag_err(const Source *src, Span *span, bool rh, const char *fmt, va_list args) {
    if (src != NULL && span != NULL) {
        fprintf(stderr, BOLD FGB_WHITE "%s:%zu:%zu: " RESET, src->file_path,
                span->start.ln, span->start.cn + 1);
    }
    verror(fmt, args);
    if (src != NULL && span != NULL) diag_render_span(src, span, rh);
}

void diag_err(const Source *src, Span *span, bool rh, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vdiag_err(src, span, rh, fmt, args);
    va_end(args);
}

void error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    verror(fmt, args);
    va_end(args);
}

static void vfatal(const char *fmt, va_list args) {
    fputs(BOLD FGB_RED "FATAL: " FGB_WHITE, stderr);
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
}

[[noreturn]] void fatal(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfatal(fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
