#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "error/error.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "shi_file.h"
#include "shi_flags.h"
#include "util/util.h"

int main(int argc, char *argv[]) {
  bool *help = shi_flag_bool("-help", false, "show the usage.");
  shi_flag_set_short(help, "h");

  bool *tok = shi_flag_bool("-tok", false, "enable verbose tokens output.");
  shi_flag_set_short(tok, "t");

  char **file = shi_flag_str("-file", NULL, "file name to read.");
  shi_flag_set_short(file, "i");

  bool arguments = shi_flag_parse(argc, argv);
  const char *program = shi_flag_program_name();

  if (!arguments) {
    error("USAGE:", format("%s [COMMAND] [OPTIONS]", program));
    shi_flag_print_options(stderr);
    error(program, "invalid arguments.");
    return 1;
  }
  if (!*file) {
    error(program, "no input files. (try [--help, -h])");
    return 1;
  }

  size_t len;
  char *buffer = shi_file_read(*file, &len);

  Error *e = init_error();

  Lexer *l = init_lexer(buffer);
  lexer(l, len, e);

  Parser *p = init_parser(l);
  parse(p);

  free_parser(p);

  if (*tok) {
    print_tokens(l);
  }
  free_tokens(l);

  add_error(e, (ErrorT){.kind = END_OF_ERROR});
  print_errors(e, *file, buffer, len);
  free(buffer);
  free_errors(e);

  return 0;
}

// SHI IMPLEMENTATIONS
#define SHI_HS_IMPLEMENTATION
#include "shi_hs.h"
#define SHI_ARENA_IMPLEMENTATION
#include "shi_arena.h"
#define SHI_OPA_IMPLEMENTATION
#include "shi_opa.h"
#define SHI_FILE_IMPLEMENTATION
#include "shi_file.h"
#define SHI_FLAGS_IMPLEMENTATION
#include "shi_flags.h"
