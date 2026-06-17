#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "parser/parser.h"

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
  print_tokens(token_pool);
  size_t errors = print_error_token_kind(argv[1], token_pool);

  if (errors) {
    free_token_pool(token_pool);
    return errors;
  }

  parser(token_pool);

  free_token_pool(token_pool);

  return 0;
}
