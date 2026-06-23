#ifndef TRIDENT_ERROR_H
#define TRIDENT_ERROR_H

#include <stddef.h>

#include "shi_opa.h"

typedef enum {
  // Lexer Error
  UNKNOWN_TOKEN,
  UNKNOWN_DIRECTIVE,
  UNTERMINATED_STRING,
  // Misc
  END_OF_ERROR,
} ErrorKind;

typedef struct {
  ErrorKind kind;
  size_t line;
  size_t col;
} ErrorT;

typedef struct {
  char **line_stream;
  size_t line_stream_len;
  SHI_OPA *error_pool_head;
  SHI_OPA *error_pool;
} Error;

Error *init_error();
void print_errors(Error *e, const char *file, const char *buffer, size_t buffer_len);
void add_error(Error *e, ErrorT err);
void free_errors(Error *e);

#endif // TRIDENT_ERROR_H
