#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/mem/mem.h"

typedef struct {
    const char *file_path;
    const char *buffer;
    size_t buffer_len;
    char *content;
    char **lines;
    size_t nlines;
} Source;

typedef struct {
    size_t ln;
    size_t cn;
    size_t i;
} Location;

typedef struct {
    Location start;
    Location end;
} Span;

Source *init_source(const char *file_path);
void free_source(Source *src);

static inline Location location(size_t ln, size_t cn, size_t i) {
    return (Location){ln, cn, i};
}

Span *span(Arena *arena, Location start, Location end);

static inline Span *span_merge(Span *a, Span *b) {
    *a = (Span){a->start, b->end};
    return a;
}
