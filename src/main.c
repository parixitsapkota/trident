#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "token/token.h"
#include "utils/utils.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "USAGE : %s <filename>", argv[0]);
    exit(1);
  }

  char *buffer = s_read_file(argv[1]);

  Tokens tokens = lexer(buffer);

  s_free(buffer);

  print_error_tokens(&tokens);

  free_tokens(&tokens);

  return 0;
}
