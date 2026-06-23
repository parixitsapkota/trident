#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

bool issymbol(const char c) {
  if ((c >= 0x21 && c <= 0x2F) || (c >= 0x3A && c <= 0x40) || (c >= 0x5B && c <= 0x60) || (c >= 0x7B && c <= 0x7E)) {
    return true;
  }
  return false;
}
