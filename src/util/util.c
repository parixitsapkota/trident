#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

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

char *format(const char *fmt, ...) {
  static char str[1024];
  va_list args;

  va_start(args, fmt);
  vsnprintf(str, sizeof(str), fmt, args);
  va_end(args);

  return str;
}

void error(const char *about, const char *discription) {
  fprintf(stderr, BOLD FG_BLACK BG_WHITE " %s " BOLD FGB_WHITE BG_RED " ERROR " RESET " %s\n", about, discription);
}

void info(const char *about, const char *discription) {
  fprintf(stdout, BOLD FG_BLACK BG_WHITE " %s " BOLD FG_BLACK BG_BLUE " INFO " RESET " %s\n", about, discription);
}

void warn(const char *about, const char *discription) {
  fprintf(stdout, BOLD FG_BLACK BG_WHITE " %s " BOLD FG_BLACK BG_YELLOW " WARNING " RESET " %s\n", about, discription);
}

void tri_log(FILE *fd, const char *stamp, const char *discription) { fprintf(fd, "[%s]: %s\n", stamp, discription); }
