#include "lexer.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct keywords {
  const char *name;
  TokenKind kind;
};

static struct keywords wordlist[] = {
    // keywords
    {"", 0},
    {"", 0},
    {"u8", U8},
    {"u64", U64},
    {"", 0},
    {"", 0},
    {"", 0},
    {"i8", I8},
    {"i64", I64},
    {"", 0},
    {"usize", USIZE},
    {"static", STATIC},
    {"do", DO},
    {"f64", F64},
    {"true", TRUE},
    {"isize", ISIZE},
    {"struct", STRUCT},
    {"if", IF},
    {"u16", U16},
    {"enum", ENUM},
    {"union", UNION},
    {"return", RET},
    {"fn", FUNC},
    {"i16", I16},
    {"else", ELSE},
    {"", 0},
    {"extern", EXTERN},
    {"default", DEFAULT},
    {"for", FOR},
    {"loop", LOOP},
    {"error", ERROR},
    {"switch", SWITCH},
    {"", 0},
    {"register", REGISTER},
    {"void", VOID},
    {"false", FALSE},
    {"inline", INLINE},
    {"", 0},
    {"var", VAR},
    {"case", CASE},
    {"while", WHILE},
    {"", 0},
    {"", 0},
    {"", 0},
    {"u128", U128},
    {"", 0},
    {"", 0},
    {"", 0},
    {"volatile", VOLATILE},
    {"i128", I128},
    {"const", CONST},
    {"", 0},
    {"", 0},
    {"continue", CONTINUE},
    {"f128", F128},
    {"", 0},
    {"", 0},
    {"", 0},
    {"u32", U32},
    {"bool", BOOL},
    {"", 0},
    {"", 0},
    {"", 0},
    {"i32", I32},
    {"type", TYPE},
    {"break", BREAK},
    {"", 0},
    {"", 0},
    {"f32", F32},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"pub", PUB}
    // keywords
};

static uint8_t weight_table[] = {
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 15, 25, 30, 0,  74, 0,  74, 0,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 5,  45, 30, 5,  5,  10, 10, 10, 5,  74, 74, 15, 74, 10, 5,  25, 74,
    10, 0,  0,  0,  20, 20, 15, 35, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
    74, 74, 74, 74, 74, 74, 74, 74, 74};

uint8_t keyword_hash(const char *str, size_t len) {
  uint8_t hash = len;

  switch (hash) {
  default:
    hash += weight_table[(uint8_t)str[2]];
  case 2:
    hash += weight_table[(uint8_t)str[1]];
  case 1:
    hash += weight_table[(uint8_t)str[0]];
  }
  return hash;
}

TokenKind get_keyword_kind(const char *str, size_t len) {
  if (len <= 8 && len >= 2) {
    uint8_t key = keyword_hash(str, len);

    if (key <= 73) {

      const char *s = wordlist[key].name;
      if (*str == *s && !strcmp(str + 1, s + 1))
        return wordlist[key].kind;
    }
  }

  return IDENTIFIER;
}
