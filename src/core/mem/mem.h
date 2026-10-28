#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t n);
void *xrealloc(void *old, size_t n);
char *xstrndup(const char *s, size_t n);
char *xstrdup(const char *s);

#ifndef ARENA_REGION_DEFAULT_CAPACITY
#define ARENA_REGION_DEFAULT_CAPACITY (1024)
#endif

typedef struct Arena Arena;

Arena *init_arena(size_t region_size);
void *arena_alloc(Arena *arena, size_t size);
void free_arena(Arena *arena);

char *substr(Arena *a, const char *source, size_t start, size_t length);
