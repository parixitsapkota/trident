#pragma once

#include <stdarg.h>

#include "source.h"

void diag_err(const Source *src, Span *span, bool rh, const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));
void vdiag_err(const Source *src, Span *span, bool rh, const char *fmt, va_list args);
void error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void verror(const char *fmt, va_list args);
[[noreturn]] void fatal(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
