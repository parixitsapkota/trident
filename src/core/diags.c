#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diags.h"
#include "def.h"

#include "syntax/lexer/lexer.h"
#include "syntax/token/token.h"

#define TAB_WIDTH 4
#define MAX_MID_LINES 4

static void put_n(char character, size_t count) {
    while (count--)
        fputc(character, stderr);
}

static int digits(size_t number) {
    int digit_count = 1;
    while (number >= 10) {
        number /= 10;
        ++digit_count;
    }
    return digit_count;
}

static size_t vcol(const char *line, size_t char_number) {
    size_t visual_column = 0, index = 0;
    for (; index < char_number && line[index]; ++index)
        visual_column +=
            (line[index] == '\t') ? TAB_WIDTH - visual_column % TAB_WIDTH : 1;
    return visual_column + (char_number - index);
}

static char *expand_tabs(const char *string) {
    char *expanded = malloc(strlen(string) * TAB_WIDTH + 1);
    size_t visual_column = 0;
    for (; *string; ++string) {
        if (*string == '\t') {
            size_t space_count = TAB_WIDTH - visual_column % TAB_WIDTH;
            for (size_t i = 0; i < space_count; ++i)
                expanded[visual_column + i] = ' ';
            visual_column += space_count;
        } else {
            expanded[visual_column++] = *string;
        }
    }
    expanded[visual_column] = '\0';
    return expanded;
}

static void put_highlighted(const Source *origin, const char *line_text) {
    char *line_string = expand_tabs(line_text);
    Source *source = init_source(origin->file_path);
    Lexer *lexer = init_lexer(source);
    Token *tokens = lex(lexer, true, line_string);
    print_tokens(tokens);
    free_lexer(lexer);
    free_source(source);
    free(line_string);
    fputc('\n', stderr);
}

static void gutter(int width, size_t line_number) {
    if (line_number)
        fprintf(stderr, DIM " %*zu | " RESET, width, line_number);
    else
        fprintf(stderr, DIM " %*s | " RESET, width, "");
}

static void render_mid(const Source *source, int width, const char *color,
                       size_t line_number, const char *line_text) {
    gutter(width, line_number);
    fprintf(stderr, "%s|" RESET " ", color);
    put_highlighted(source, line_text);
}

static void diag_render_span(const Source *source, const Span *span, bool red_highlight) {
    size_t start_line = span->start.ln;
    size_t end_line = span->end.ln < start_line ? start_line : span->end.ln;

    if (start_line == 0 || end_line > source->nlines)
        fatal("incorrect line range: %zu-%zu for src: %s", start_line, end_line,
              source->file_path);

    const char *color = red_highlight ? FGB_RED : FGB_GREEN;
    const char *start_line_text = source->lines[start_line - 1];
    int width = digits(end_line);
    size_t start_column = vcol(start_line_text, span->start.cn);

    if (end_line == start_line) {
        size_t end_column = vcol(start_line_text, span->end.cn);
        size_t tilde_count =
            end_column > start_column + 1 ? end_column - start_column - 1 : 0;
        --start_column;

        gutter(width, start_line);
        put_highlighted(source, start_line_text);
        gutter(width, 0);
        put_n(' ', start_column);
        fputs(color, stderr);
        fputc('^', stderr);
        put_n('~', tilde_count);
        fputs(RESET "\n", stderr);
        return;
    }

    const char *end_line_text = source->lines[end_line - 1];
    size_t last_char_index = span->end.cn ? span->end.cn - 1 : 0;
    size_t end_column = vcol(end_line_text, last_char_index);

    gutter(width, start_line);
    fputs("  ", stderr);
    put_highlighted(source, start_line_text);

    gutter(width, 0);
    fputc(' ', stderr);
    fputs(color, stderr);
    put_n('_', start_column);
    fputs("^" RESET "\n", stderr);

    size_t middle_start = start_line + 1, middle_end = end_line - 1;
    if (middle_end >= middle_start) {
        if (middle_end - middle_start + 1 > MAX_MID_LINES) {
            render_mid(source, width, color, middle_start,
                       source->lines[middle_start - 1]);
            render_mid(source, width, color, middle_start + 1,
                       source->lines[middle_start]);
            fprintf(stderr, " %*s\n", width < 3 ? 3 : width, "...");
            render_mid(source, width, color, middle_end, source->lines[middle_end - 1]);
        } else {
            for (size_t line = middle_start; line <= middle_end; ++line)
                render_mid(source, width, color, line, source->lines[line - 1]);
        }
    }

    render_mid(source, width, color, end_line, end_line_text);

    gutter(width, 0);
    fputs(color, stderr);
    fputc('|', stderr);
    put_n('_', end_column);
    fputs("^" RESET "\n", stderr);
}

void verror(const char *format, va_list arguments) {
    fputs(BOLD FGB_RED "ERROR: " FGB_WHITE, stderr);
    vfprintf(stderr, format, arguments);
    fputs(RESET "\n", stderr);
}

void vdiag_err(const Source *source, Span *span, bool red_highlight, const char *format,
               va_list arguments) {
    if (source != NULL && span != NULL) {
        fprintf(stderr, BOLD FGB_WHITE "%s:%zu:%zu: " RESET, source->file_path,
                span->start.ln, span->start.cn + 1);
    }
    verror(format, arguments);
    if (source != NULL && span != NULL) diag_render_span(source, span, red_highlight);
}

void diag_err(const Source *source, Span *span, bool red_highlight, const char *format,
              ...) {
    va_list arguments;
    va_start(arguments, format);
    vdiag_err(source, span, red_highlight, format, arguments);
    va_end(arguments);
}

void error(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    verror(format, arguments);
    va_end(arguments);
}

static void vfatal(const char *format, va_list arguments) {
    fputs(BOLD FGB_RED "FATAL: " FGB_WHITE, stderr);
    vfprintf(stderr, format, arguments);
    fputs(RESET "\n", stderr);
}

[[noreturn]] void fatal(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    vfatal(format, arguments);
    va_end(arguments);
    exit(EXIT_FAILURE);
}
