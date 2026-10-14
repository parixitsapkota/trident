#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *file_path;
    const char *buffer;
    size_t buffer_len;
    char *content;
    char **lines;
    size_t nlines;
} Source;

Source *init_source(const char *file_path);
void free_source(Source *src);
