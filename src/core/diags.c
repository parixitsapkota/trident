
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "diags.h"

#include "def.h"

static void diag_render_line(const Source *src, Span span, bool rh) {

    size_t line = span.start.ln;

    if (line > src->nlines || line == 0) {
        fatal("incorrect line number: %zu for src: %s", line, src->file_path);
    }

    fprintf(stderr, " %5zu | %-72s\n", line, src->lines[line - 1]);
    fprintf(stderr, "       |");
    for (size_t i = 0; i < span.start.cn; ++i)
        fputc(' ', stderr);
    size_t cn_len = span.end.cn - span.start.cn - 1;
    const char *col = rh ? FGB_RED : FGB_GREEN;
    fprintf(stderr, RESET "%s", col);
    fputc('^', stderr);
    for (size_t i = 0; i < cn_len; ++i)
        fputc('~', stderr);
    fprintf(stderr, RESET "\n");
}

void verror(const char *fmt, va_list args) {
    fputs(BOLD FGB_RED "ERROR: " FGB_WHITE, stderr);
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
}

void vdiag_err(const Source *src, Span span, bool rh, const char *fmt, va_list args) {
    fprintf(stderr, BOLD FGB_WHITE "%s:%zu:%zu: " RESET, src->file_path, span.start.ln,
            span.start.cn);
    verror(fmt, args);
    diag_render_line(src, span, rh);
}

void diag_err(const Source *src, Span span, bool rh, const char *fmt, ...) {
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
