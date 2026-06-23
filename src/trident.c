#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error/error.h"
#include "lexer/lexer.h"
#include "shi_file.h"
#include "shi_flags.h"
#include "util/util.h"

int main(int argc, char *argv[]) {
  bool *tok = shi_flag_bool("-tok", false, "enable verbose tokens output.");
  shi_flag_set_short(tok, "t");
  char **file = shi_flag_str("-file", NULL, "file name to read.");
  shi_flag_set_short(file, "i");

  if (!shi_flag_parse(argc, argv)) {
    shi_flag_print_error(stderr);
    fprintf(stderr, "Usage: %s [OPTIONS]\n", shi_flag_program_name());
    shi_flag_print_options(stderr);
    return 1;
  }

  if (!*file) {
    fprintf(stderr, BOLD FG_BLACK BG_WHITE "%s: " FG_WHITE BG_RED " ERROR " RESET " no input files.\n", shi_flag_program_name());
    exit(1);
  }

  size_t len;
  char *buffer = shi_file_read(*file, &len);
  Error *e = init_error();
  Lexer *l = init_lexer(buffer);
  lexer(l, len, e);

  if (*tok) {
    print_tokens(l);
  }
  print_errors(e, *file, buffer, len);

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
#define SHI_FLAGS_IMPLEMENTATION
#include "shi_flags.h"
