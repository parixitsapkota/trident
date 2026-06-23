#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error/error.h"
#include "lexer/lexer.h"
#include "shi_file.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <FILE>\n", argv[0]);
    return 1;
  }

  size_t len;
  char *buffer = shi_file_read(argv[1], &len);
  Error *e = init_error();
  Lexer *l = init_lexer(buffer);
  lexer(l, len, e);

  // print_tokens(l);
  print_errors(e, argv[1], buffer, len);

  free_errors(e);
  free(buffer), (void)len;
  free_tokens(l);

  return 0;
}

// SHI IMPLEMENTATIONS
#define SHI_HS_IMPLEMENTATION
#include "shi_hs.h"
#define SHI_OPA_IMPLEMENTATION
#include "shi_opa.h"
#define SHI_FILE_IMPLEMENTATION
#include "shi_file.h"
