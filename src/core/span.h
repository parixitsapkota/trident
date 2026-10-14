#pragma once

#include <stddef.h>

#include "arena.h"

typedef struct {
    size_t ln;
    size_t cn;
    size_t i;
} Location;

typedef struct {
    Location start;
    Location end;
} Span;

static inline Location location(size_t ln, size_t cn, size_t i) {
    return (Location){ln, cn, i};
}

Span *span(Arena *arena, Location start, Location end);

static inline Span span_merge(Span a, Span b) { return (Span){a.start, b.end}; }
