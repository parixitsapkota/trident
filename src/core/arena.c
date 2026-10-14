#include <stdlib.h>

#include "arena.h"

#include "mem.h"

typedef struct ArenaRegion ArenaRegion;

struct ArenaRegion {
    ArenaRegion *next;
    size_t offset;
    size_t cap;
    uintptr_t data[];
};

struct Arena {
    size_t region_size;
    ArenaRegion *begin;
    ArenaRegion *current;
    ArenaRegion *end;
};

static int bytes_to_words(size_t bytes, size_t *out_words) {
    if (bytes > SIZE_MAX - (sizeof(uintptr_t) - 1)) {
        return 0;
    }
    *out_words = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
    return 1;
}

static ArenaRegion *init_arena_region(size_t capacity) {
    if (capacity > (SIZE_MAX - sizeof(ArenaRegion)) / sizeof(uintptr_t)) {
        return NULL;
    }

    size_t size_bytes = sizeof(ArenaRegion) + sizeof(uintptr_t) * capacity;
    ArenaRegion *region = xmalloc(size_bytes);
    if (!region) {
        return NULL;
    }

    region->next = NULL;
    region->offset = 0;
    region->cap = capacity;

    return region;
}

static int push_new_arena_region(Arena *arena, size_t min_cap) {
    if (!arena) {
        return 0;
    }

    size_t cap = arena->region_size;
    if (cap < min_cap) {
        cap = min_cap;
    }

    ArenaRegion *region = init_arena_region(cap);
    if (!region) {
        return 0;
    }

    if (!arena->end) {
        arena->begin = region;
        arena->current = region;
        arena->end = region;
    } else {
        arena->end->next = region;
        arena->end = region;
        arena->current = region;
    }

    return 1;
}

Arena *init_arena(size_t region_size_bytes) {
    Arena *arena = (Arena *)xmalloc(sizeof(Arena));
    if (!arena) {
        return NULL;
    }

    size_t capacity_words = 0;
    if (region_size_bytes > 0) {
        if (!bytes_to_words(region_size_bytes, &capacity_words)) {
            free(arena);
            return NULL;
        }
    } else {
        capacity_words = ARENA_REGION_DEFAULT_CAPACITY;
    }

    *arena = (Arena){
        .region_size = capacity_words,
        .begin = NULL,
        .current = NULL,
        .end = NULL,
    };

    return arena;
}

void *arena_alloc(Arena *arena, size_t size_bytes) {
    if (!arena || size_bytes == 0) {
        return NULL;
    }

    size_t size_words = 0;
    if (!bytes_to_words(size_bytes, &size_words)) {
        return NULL;
    }

    if (!arena->current) {
        if (!push_new_arena_region(arena, size_words)) {
            return NULL;
        }
    }

    while (arena->current->offset > arena->current->cap ||
           size_words > arena->current->cap - arena->current->offset) {

        if (arena->current->offset > arena->current->cap) {
            return NULL;
        }

        if (arena->current->next == NULL) {
            if (!push_new_arena_region(arena, size_words)) {
                return NULL;
            }
            break;
        }

        arena->current = arena->current->next;
    }

    if (arena->current->cap - arena->current->offset < size_words) {
        if (!push_new_arena_region(arena, size_words)) {
            return NULL;
        }
    }

    void *dest = &arena->current->data[arena->current->offset];
    arena->current->offset += size_words;
    return dest;
}

void free_arena(Arena *arena) {
    if (!arena) {
        return;
    }

    ArenaRegion *region = arena->begin;
    while (region) {
        ArenaRegion *next = region->next;
        free(region);
        region = next;
    }

    free(arena);
}
