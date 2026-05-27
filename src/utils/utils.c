#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *s_malloc(const size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "FATAL : Memory allocation failed for size %zu\n", size);
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void *s_realloc(const void *ptr, const size_t new_size) {
  if (!ptr) {
    fprintf(stderr, "FATAL : Can't reallocate a NULL ptr!\n");
    exit(EXIT_FAILURE);
  }
  void *new_ptr = realloc((void *)ptr, new_size);
  if (!new_ptr) {
    fprintf(stderr, "FATAL : Memory reallocation failed for size %zu\n",
            new_size);
    exit(EXIT_FAILURE);
  }
  return new_ptr;
}

char *s_strdup(const char *str) {
  char *new_str = strdup(str);
  if (!new_str) {
    fprintf(stderr, "FATAL : String duplication failed!\n");
    exit(EXIT_FAILURE);
  }
  return new_str;
}

char *s_substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = s_malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

char *s_read_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "FATAL : Failed to open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  const size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = s_malloc(file_size + 1);

  const size_t bytes = fread(content, 1, file_size, file);
  if (bytes != file_size) {
    fprintf(stderr, "FATAL : Failed to read bytes form file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  content[file_size] = '\0';

  fclose(file);
  return content;
}

void s_free(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "WARNING : Can't free a NULL ptr!\n");
    return;
  }
  free(ptr);
  ptr = NULL;
}
