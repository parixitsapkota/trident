#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"

#include "arena.h"
#include "diags.h"

void *xmalloc(size_t n) {
    void *p = malloc(n ? n : 1);
    if (!p) fatal("Out of memory");
    return p;
}

void *xrealloc(void *old, size_t n) {
    void *p = realloc(old, n ? n : 1);
    if (!p) fatal("Out of memory");

    return p;
}

char *xstrndup(const char *s, size_t n) {
    char *p = xmalloc(n + 1);
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

char *substr(Arena *a, const char *source, size_t start, size_t end) {
    const size_t length = end > start ? end - start : 0;
    char *out = arena_alloc(a, length + 1);
    memcpy(out, source + start, length);
    out[length] = '\0';
    return out;
}
