#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

void *xmalloc(size_t n);
void *xrealloc(void *old, size_t n);
char *xstrndup(const char *s, size_t n);
char *xstrdup(const char *s);
char *substr(Arena *a, const char *source, size_t start, size_t length);
