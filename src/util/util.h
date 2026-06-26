#ifndef TRIDENT_UTILS_H
#define TRIDENT_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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
/* Bright Foreground Colors */
#define FGB_BLACK "\033[90m"
#define FGB_RED "\033[91m"
#define FGB_GREEN "\033[92m"
#define FGB_YELLOW "\033[93m"
#define FGB_BLUE "\033[94m"
#define FGB_MAGENTA "\033[95m"
#define FGB_CYAN "\033[96m"
#define FGB_WHITE "\033[97m"
/* Bright Background Colors */
#define BGB_BLACK "\033[100m"
#define BGB_RED "\033[101m"
#define BGB_GREEN "\033[102m"
#define BGB_YELLOW "\033[103m"
#define BGB_BLUE "\033[104m"
#define BGB_MAGENTA "\033[105m"
#define BGB_CYAN "\033[106m"
#define BGB_WHITE "\033[107m"

char *substr(const char *str, const size_t start, const size_t end);
bool issymbol(const char c);

char *format(const char *format, ...);

void error(const char *about, const char *discription);
void info(const char *about, const char *discription);
void warn(const char *about, const char *discription);
void tri_log(FILE *fd, const char *stamp, const char *discription);

#endif // TRIDENT_UTILS_H
