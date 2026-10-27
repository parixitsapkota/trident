#pragma once

#ifndef VERSION_INFO
#define VERSION_INFO ""
#endif // VERSION_INFO

#ifndef TIME_INFO
#define TIME_INFO ""
#endif // TIME_INFO

#ifndef CC_INFO
#define CC_INFO ""
#endif // CC_INFO

// Reset
#define RESET "\033[0m"
// Text Formatting
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN "\033[8m"
#define STRIKETHROUGH "\033[9m"
// Foreground Colors (30-37)
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
#define FG_DEFAULT "\033[39m"
// Background Colors (40-47)
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"
#define BG_DEFAULT "\033[49m"
// Bright Foreground Colors (90-97)
#define FGB_BLACK "\033[90m"
#define FGB_RED "\033[91m"
#define FGB_GREEN "\033[92m"
#define FGB_YELLOW "\033[93m"
#define FGB_BLUE "\033[94m"
#define FGB_MAGENTA "\033[95m"
#define FGB_CYAN "\033[96m"
#define FGB_WHITE "\033[97m"
// Bright Background Colors (100-107)
#define BGB_BLACK "\033[100m"
#define BGB_RED "\033[101m"
#define BGB_GREEN "\033[102m"
#define BGB_YELLOW "\033[103m"
#define BGB_BLUE "\033[104m"
#define BGB_MAGENTA "\033[105m"
#define BGB_CYAN "\033[106m"
#define BGB_WHITE "\033[107m"

#define FG256(n) "\033[38;5;" #n "m"

#define COL_TYPE FG256(80)
#define COL_CONTROL FG256(176)
#define COL_DECL FG256(110)
#define COL_BOOL FG256(209)
#define COL_INTRINSIC FG256(222)
#define COL_NUMBER FG256(215)
#define COL_STRING FG256(150)
#define COL_CHAR FG256(151)
#define COL_PUNCT FG256(246)
#define COL_IDENT FG256(253)
#define COL_COMMENT FG256(243) ITALIC
#define COL_UNKNOWN FG256(203) UNDERLINE
