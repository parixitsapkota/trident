#ifndef TRIDENT_UTILS_H
#define TRIDENT_UTILS_H

#include <stdbool.h>
#include <stddef.h>

/* Reset */
#define RESET "\033[0m"
/* Text Formatting */
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define STRIKETHROUGH "\033[9m"
/* Foreground Colors */
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
/* Background Colors */
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

char *substr(const char *str, const size_t start, const size_t end);
bool issymbol(const char c);

#endif // TRIDENT_UTILS_H
