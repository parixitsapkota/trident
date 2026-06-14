#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../SHI/shi_file.h"
#include "lexer/lexer.h"
#include "token/token.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "USAGE : %s <filename>", argv[0]);
    exit(1);
  }

  char *buffer = shi_file_read(argv[1], NULL);

  Tokens tokens = lexer(buffer);

  free(buffer);

  print_error_tokens(&tokens);

  return 0;
}
