#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "shi_opa.h"
#include "util/util.h"

Error *init_error() {
  static Error e = {0};
  e.error_pool_head = shi_opa_init(ErrorT, 50);
  e.error_pool = e.error_pool_head;
  return &e;
}

void init_line_stream(Error *e, const char *buffer, const size_t len) {
  size_t lines = 0;
  for (size_t i = 0; i < len; ++i) {
    if (buffer[i] == '\n') {
      ++lines;
    }
  }
  if (len > 0 && buffer[len - 1] != '\n') {
    ++lines;
  }

  e->line_stream_len = lines;
  e->line_stream = malloc(sizeof(char *) * lines);

  size_t start = 0;
  size_t current_line = 0;

  for (size_t i = 0; i < len; ++i) {
    if (buffer[i] == '\n') {
      const size_t line_length = i - start;
      e->line_stream[current_line] = substr(buffer, start, start + line_length);
      ++current_line;
      start = i + 1;
    }
  }
  if (start < len) {
    const size_t line_length = len - start;
    e->line_stream[current_line] = substr(buffer, start, start + line_length);
  }
}

#define err_str(a, b)                                                                                                          \
  case a: return b

static const char *error_kind_to_str(const ErrorKind kind) {
  switch (kind) {
    err_str(UNKNOWN_TOKEN, "Unexpected token.");
    err_str(UNKNOWN_DIRECTIVE, "Unknown directive.");
    err_str(UNTERMINATED_STRING, "Unterminated quote.");
  default: return "";
  }
}

void print_errors(Error *e, const char *file, const char *buffer, const size_t buffer_len) {
  init_line_stream(e, buffer, buffer_len);

  size_t errors = 0;
  for (size_t i = 0;; ++i) {
    ErrorT *err = shi_opa_index(e->error_pool_head, i);
    if (!err || err->kind == END_OF_ERROR) {
      break;
    }

    ++errors;
    const char *msg = error_kind_to_str(err->kind);
    error(format("%s:%lu:%lu:", file, err->line, err->col), msg);

    if (e->line_stream && err->line > 0 && err->line <= e->line_stream_len) {
      const char *format = BOLD FG_WHITE "%4lu|" RESET " %s\n";
      printf(format, err->line, e->line_stream[err->line - 1]);
    }
    printf(BOLD FG_WHITE "    |" RESET);
    for (size_t i = 0; i < err->col; ++i) {
      putchar(' ');
    }
    printf(BOLD FG_GREEN "^~" RESET "\n");
  }
  if (errors != 0) {
    info(file, format("reported %lu error(s).", errors));
  }
}

void add_error(Error *e, ErrorT err) { shi_opa_push(e->error_pool, err); }

void free_errors(Error *e) {
  if (e->line_stream) {
    for (size_t i = 0; i < e->line_stream_len; ++i) {
      free(e->line_stream[i]);
    }
    free(e->line_stream);
  }
  e->line_stream_len = 0;
  free_mem_chain(e->error_pool_head);
  e->error_pool_head = NULL;
  e->error_pool = NULL;
}
