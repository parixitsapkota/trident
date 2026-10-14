#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef ARENA_REGION_DEFAULT_CAPACITY
#define ARENA_REGION_DEFAULT_CAPACITY (1024)
#endif

typedef struct Arena Arena;

Arena *init_arena(size_t region_size);
void *arena_alloc(Arena *arena, size_t size);
void free_arena(Arena *arena);
