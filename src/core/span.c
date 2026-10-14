#include "span.h"

#include "arena.h"

Span *span(Arena *arena, Location start, Location end) {
    Span *s = arena_alloc(arena, sizeof(Span));
    *s = (Span){start, end};
    return s;
}
