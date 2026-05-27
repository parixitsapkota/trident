#ifndef ANVIL_UTILS_H
#define ANVIL_UTILS_H

#include <stddef.h>

#undef NULL
#define NULL ((void *)0)

#undef true
#undef false
typedef unsigned short bool;
#define true 1
#define false 0

void *s_malloc(size_t size);
void *s_realloc(const void *ptr, size_t new_size);
char *s_strdup(const char *str);
char *s_substr(const char *str, size_t start, size_t length);
char *s_read_file(const char *filename);
void s_free(void *ptr);

#endif // ANVIL_UTILS_H
