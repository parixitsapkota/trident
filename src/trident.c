#include "lexer/lexer.h"
#include "parser/parser.h"

#define SHI_OPA_IMPLEMENTATION
#include "../SHI/shi_opa.h"
#define SHI_FILE_IMPLEMENTATION
#include "../SHI/shi_file.h"
#define SHI_HS_IMPLEMENTATION
#include "../SHI/shi_hs.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "USAGE : %s <filename>\n", argv[0]);
    exit(1);
  }

  char *buffer = read_file(argv[1], NULL);

  LexReturn lexret = lexer(buffer);
  free(buffer);

  print_tokens(lexret.token_pool);
  size_t errors = print_error_token_kind(argv[1], lexret.token_pool);

  if (errors) {
    free_token_pool(lexret.token_pool);
    free_interned_symbols(lexret.set);
    shi_hs_free(lexret.set);
    return errors;
  }

  parser(&lexret);

  free_token_pool(lexret.token_pool);
  free_interned_symbols(lexret.set);
  shi_hs_free(lexret.set);

  return 0;
}
