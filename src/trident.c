#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"

#define SHI_OPA_IMPLEMENTATION
#include "../SHI/shi_opa.h"
#define SHI_FILE_IMPLEMENTATION
#include "../SHI/shi_file.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "USAGE : %s <filename>\n", argv[0]);
    exit(1);
  }

  char *buffer = read_file(argv[1], NULL);

  SHI_OPA *token_pool = lexer(buffer);
  free(buffer);

  size_t errors = print_error_token_kind(argv[1], token_pool);
  if (errors)
    return errors;

  return 0;
}
