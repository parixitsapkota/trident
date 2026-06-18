#ifndef TRIDENT_PARSER_H
#define TRIDENT_PARSER_H

#include "../../SHI/shi_opa.h"

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  // Keywords
  // Qualifier
  PUB,
  EXTERN,
  INLINE,
  STATIC,
  REGISTER,
  CONST,
  VAR,
  VOLATILE,
  // Function
  FUNC,
  RET,
  // Control flow
  IF,
  ELSE,
  LOOP,
  DO,
  WHILE,
  FOR,
  BREAK,
  CONTINUE,
  SWITCH,
  CASE,
  DEFAULT,
  // Types
  TYPE,
  VOID,
  // Boolean
  BOOL,
  TRUE,
  FALSE,
  // Signed_Int
  I8,
  I16,
  I32,
  I64,
  I128,
  ISIZE,
  // Unsigned_Int
  U8,
  U16,
  U32,
  U64,
  U128,
  USIZE,
  // Float
  F32,
  F64,
  F128,
  // Struct And Union
  STRUCT,
  UNION,
  // Enumerator
  ENUM,
  ERROR,
  // Identifier & Literals
  IDENTIFIER,
  INT,
  FLOAT,
  STRING,
  CHARACTER,
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
  SEMICOLON,
  COLN,
  WHAT,
  // Operator
  COMMA,
  DOT,
  DOT_DOT,
  PLUS,
  DASH,
  STAR,
  SLASH,
  PERCENT,
  // Unary
  INCREMENT,
  DECREMENT,
  NEGATE,
  // Logical
  LOGICAL_AND,
  LOGICAL_OR,
  LOGICAL_XOR,
  // Bitwise Logic
  BITWISE_NOT,
  ANDPERCENT,
  BITWISE_OR,
  BITWISE_XOR,
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
  // Assignment
  ASSIGN,
  PLUS_ASSIGN,
  DASH_ASSIGN,
  STAR_ASSIGN,
  SLASH_ASSIGN,
  PERCENT_ASSIGN,
  SHIFT_LEFT_ASSIGN,
  SHIFT_RIGHT_ASSIGN,
  AND_ASSIGN,
  OR_ASSIGN,
  XOR_ASSIGN,
  // Error
  UNTERMINATED_STRING,
  UNTERMINATED_CHAR,
  UNKNOWN_TOKEN,
  UNKNOWN_DIRECTIVE,
  // End of Tokens & unknown
  END_OF_TOKEN,
  UNKNOWN,
} TokenKind;

typedef struct {
  const char *word;
  TokenKind kind;
} KeywordMap;

typedef struct {
  TokenKind kind;
  char *value;
  size_t line;
  size_t col;
} Token;

char *token_kind_to_str(TokenKind kind);
TokenKind get_keyword_kind(const char *str, size_t len);
TokenKind get_directive_kind(const char *word);
bool is_token_kind_error(TokenKind kind);
void print_tokens(SHI_OPA *token_pool);
size_t print_error_token_kind(const char *file_name, SHI_OPA *token_pool);

SHI_OPA *lexer(const char *buffer);
void free_token_pool(SHI_OPA *token_pool);

#endif // TRIDENT_PARSER_H
