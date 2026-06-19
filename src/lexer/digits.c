#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *substr(const char *str, size_t start, size_t end);

static bool is_octal_digit(char c) { return c >= '0' && c <= '7'; }
static bool is_hex_digit(char c) {
  return isdigit((unsigned char)c) || (c >= 'A' && c <= 'F');
}

static char *get_decimal(Lexer *l, TokenKind *kind) {
  size_t start = l->index;

  while (isdigit((unsigned char)peak(l, 0)))
    consume(l);

  if (peak(l, 0) == '.') {
    *kind = FLOAT;
    consume(l); // consume '.'

    while (isdigit((unsigned char)peak(l, 0)))
      consume(l);

    if (peak(l, 0) == 'e') {
      consume(l); // consume 'e'
      if (peak(l, 0) == '+' || peak(l, 0) == '-')
        consume(l);
      while (isdigit((unsigned char)peak(l, 0)))
        consume(l);
    }
  } else {
    *kind = INT;
    if (peak(l, 0) == 'U')
      consume(l); // optional unsigned suffix
  }

  return substr(l->buffer, start, l->index);
}

static char *get_octal(Lexer *l, TokenKind *kind) {
  size_t start = l->index;

  consume(l); // '0'
  consume(l); // '_'

  while (is_octal_digit(peak(l, 0)))
    consume(l);

  bool has_dot = false;
  if (peak(l, 0) == '.') {
    has_dot = true;
    consume(l);
    while (is_octal_digit(peak(l, 0)))
      consume(l);
  }

  if (peak(l, 0) == 'p') {
    *kind = FLOAT;
    consume(l); // 'p'
    if (peak(l, 0) == '+' || peak(l, 0) == '-')
      consume(l);
    while (is_octal_digit(peak(l, 0)))
      consume(l);
  } else if (has_dot) {
    *kind = FLOAT;
  } else {
    *kind = INT;
    if (peak(l, 0) == 'U')
      consume(l);
  }

  return substr(l->buffer, start, l->index);
}

static char *get_hex(Lexer *l, TokenKind *kind) {
  size_t start = l->index;

  consume(l); // '0'
  consume(l); // 'x'

  while (is_hex_digit(peak(l, 0)))
    consume(l);

  bool has_dot = false;
  if (peak(l, 0) == '.') {
    has_dot = true;
    consume(l);
    while (is_hex_digit(peak(l, 0)))
      consume(l);
  }

  if (peak(l, 0) == 'p') {
    *kind = FLOAT;
    consume(l); // 'p'
    if (peak(l, 0) == '+' || peak(l, 0) == '-')
      consume(l);
    while (is_hex_digit(peak(l, 0)))
      consume(l);
  } else if (has_dot) {
    *kind = FLOAT;
  } else {
    *kind = INT;
    if (peak(l, 0) == 'U')
      consume(l);
  }

  return substr(l->buffer, start, l->index);
}

char *get_digit(Lexer *l, TokenKind *kind) {
  if (peak(l, 0) == '0' && peak(l, 1) == '_')
    return get_octal(l, kind);

  if (peak(l, 0) == '0' && peak(l, 1) == 'x')
    return get_hex(l, kind);

  if (peak(l, 0) == '.') {
    size_t start = l->index;
    *kind = FLOAT;
    consume(l); // '.'
    while (isdigit((unsigned char)peak(l, 0)))
      consume(l);
    if (peak(l, 0) == 'e') {
      consume(l);
      if (peak(l, 0) == '+' || peak(l, 0) == '-')
        consume(l);
      while (isdigit((unsigned char)peak(l, 0)))
        consume(l);
    }
    return substr(l->buffer, start, l->index);
  }

  return get_decimal(l, kind);
}
