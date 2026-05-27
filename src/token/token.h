#ifndef ANVIL_TOKEN_H
#define ANVIL_TOKEN_H

#include "../utils/utils.h"
#include <stddef.h>

typedef enum {
  // Helper_Tokens
  END_OF_TOKEN,
  UNKNOWN,
  // Identifier
  IDENTIFIER,
  // Literals
  INT,
  FLOAT,
  STRING,
  // Qualifier
  PUB,
  EXTERN,
  CONST,
  VOLATILE,
  // Function
  FUNC,
  RET,
  // Conditional
  IF,
  ELSE,
  // Loop
  LOOP,
  WHILE,
  // Signed_Int
  I8,
  I16,
  I32,
  I64,
  I128,
  // Unsigned_Int
  U8,
  U16,
  U32,
  U64,
  U128,
  // Float
  F16,
  F32,
  F64,
  F128,
  // Struct And Union
  STRUCT,
  UNION,
  // Enumerator
  ENUM,
  ERROR,
  // Pointers
  PTR,
  DREF,
  // Directive
  TYPEOF,
  DEFINE,
  IMPORT,
  // Seperator
  O_BRACE,
  C_BRACE,
  O_PREN,
  C_PREN,
  O_BRACKET,
  C_BRACKET,
  COMMA,
  COLN,
  SEMICOLON,
  // Operator
  PLUS,
  DASH,
  STAR,
  SLASH,
  PERCENT,
  // Assignment
  ASSIGN,
  PLUS_ASSIGN,
  DASH_ASSIGN,
  STAR_ASSIGN,
  SLASH_ASSIGN,
  PERCENT_ASSIGN,
  // Unary
  INCREMENT,
  DECREMENT,
  NEGATE,
  // Logical
  LOGICAL_AND,
  LOGICAL_OR,
  // Shift
  SHIFT_LEFT,
  SHIFT_RIGHT,
  // Compare
  EQUAL,
  NOT_EQUAL,
  LESSER,
  GREATER,
  LESSER_EQUAL,
  GREATER_EQUAL,
  // Error
  UNTERMINATED_STRING,
  UNKNOWN_TOKEN,
  UNKNOWN_DIRECTIVE
} TokenKind;

char *token_kind_to_str(TokenKind kind);

typedef struct {
  const char *word;
  TokenKind kind;
} KeywordMap;

static const KeywordMap keywords[] = {
    /*
     * bsearch keyword map
     */
    {"const", CONST},
    {"dref", DREF},
    {"else", ELSE},
    {"enum", ENUM},
    {"error", ERROR},
    {"extern", EXTERN},
    {"f128", F128},
    {"f16", F16},
    {"f32", F32},
    {"f64", F64},
    {"func", FUNC},
    {"i128", I128},
    {"i16", I16},
    {"i32", I32},
    {"i64", I64},
    {"i8", I8},
    {"if", IF},
    {"loop", LOOP},
    {"ptr", PTR},
    {"pub", PUB},
    {"ret", RET},
    {"struct", STRUCT},
    {"u128", U128},
    {"u16", U16},
    {"u32", U32},
    {"u64", U64},
    {"u8", U8},
    {"union", UNION},
    {"volatile", VOLATILE},
    {"while", WHILE}};

TokenKind get_keyword_kind(const char *word);
TokenKind get_directive_kind(const char *word);

typedef struct {
  TokenKind kind;
  char *value;
  size_t line;
  size_t col;
} Token;

typedef struct {
  Token *tokens;
  size_t token_capacity;
  size_t token_count;
} Tokens;

bool is_error_token_kind(TokenKind kind);
void print_error_tokens(const Tokens *self);
void add_token(Tokens *self, TokenKind kind, const char *value, size_t line,
               size_t col);
void free_tokens(Tokens *self);

#endif // ANVIL_TOKEN_H
