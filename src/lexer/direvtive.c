#include <string.h>

#include "token.h"

TokenKind get_directive_kind(const char *word) {
  if (!strcmp("typeof", word)) {
    return TYPEOF;
  }
  if (!strcmp("define", word)) {
    return DEFINE;
  }
  if (!strcmp("import", word)) {
    return IMPORT;
  }
  return UNKNOWN;
}