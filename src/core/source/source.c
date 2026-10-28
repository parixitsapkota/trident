#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/def.h"
#include "core/diags/diags.h"
#include "core/mem/mem.h"
#include "source.h"

static FILE *openf(const char *path, uint8_t mode) {
    const char *mode_str = mode ? "rb" : "wa";
    FILE *f = fopen(path, mode_str);
    if (!f) fatal("File not found! " FG_GREEN "\"%s\"" RESET, path);
    return f;
}

static char *readf(FILE *file, size_t *bytes) {
    fseek(file, 0, SEEK_END);
    const size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(file_size + 1);
    if (!content) fatal("Failed to allocate %zu bytes.", file_size + 1);
    const size_t bytes_red = fread(content, 1, file_size, file);
    if (bytes_red != file_size) fatal("Failed to read file.");
    content[file_size] = '\0';

    if (bytes) {
        *bytes = bytes_red;
    }

    return content;
}

Source *init_source(const char *file_path) {
    FILE *file = openf(file_path, 1);
    size_t buf_len = 0;
    const char *content = readf(file, &buf_len);

    if (!content) return NULL;

    size_t line_count = 0;
    if (buf_len > 0) {
        line_count = 1;
        for (size_t i = 0; i < buf_len; ++i) {
            if (content[i] == '\n') ++line_count;
        }
    }

    Source *s = malloc(sizeof(Source));
    s->file_path = file_path;
    s->buffer = content;
    s->buffer_len = buf_len;
    s->content = xstrndup(content, buf_len);
    s->nlines = line_count;
    s->lines = (char **)malloc(sizeof(char *) * line_count);

    if (line_count > 0) {
        size_t line_idx = 0;
        s->lines[line_idx++] = s->content;
        for (size_t i = 0; i < buf_len; ++i) {
            if (s->content[i] == '\n') {
                s->content[i] = '\0';
                if (line_idx < line_count) s->lines[line_idx++] = &s->content[i + 1];
            }
        }
    }

    return s;
}

void free_source(Source *s) {
    if (!s) return;
    free(s->content);
    free((char *)s->lines);
    free((char *)s->buffer);
    free(s);
}

Span *span(Arena *arena, Location start, Location end) {
    Span *s = arena_alloc(arena, sizeof(Span));
    *s = (Span){start, end};
    return s;
}
