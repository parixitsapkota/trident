#ifndef TRIDENT_TOKEN_H
#define TRIDENT_TOKEN_H

#include <float.h>
#include <stdint.h>

typedef enum {
  // Misc Tokens
  UNKNOWN = 0,
  END_OF_TOKEN,

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
  O_BRACE,   // `{`
  C_BRACE,   // `}`
  O_PREN,    // `(`
  C_PREN,    // `)`
  O_BRACKET, // `[`
  C_BRACKET, // `]`
  SEMICOLON, // `;`
  // Operator
  COMMA, // `,`
  ACESS, // `.`
  ADD,   // `+`
  SUB,   // `-`
  DEV,   // `/`
  MOD,   // `%`
  // Range
  INCLUSIVE_RANGE, // `..`

  // Unary
  INCREMENT, // `++`
  DECREMENT, // `--`
  NEGATE,    // `!`
  // Logical
  LOGICAL_AND, // `&&`
  LOGICAL_OR,  // `||`
  LOGICAL_XOR, // `^^`
  // Bitwise Logic
  BIT_NOT, // `~`
  BIT_OR,  // `|`
  BIT_XOR, // `^`
  // Shift
  SHIFT_LEFT,  // `<<`
  SHIFT_RIGHT, // `>>`
  // Compare
  EQUAL,         // `=`
  NOT_EQUAL,     // `!=`
  LESSER,        // `<`
  GREATER,       // `>`
  LESSER_EQUAL,  // `<=`
  GREATER_EQUAL, // `>=`
  // Assignment
  ASSIGN,             // `=`
  ADD_ASSIGN,         // `+=`
  SUB_ASSIGN,         // `-=`
  MUL_ASSIGN,         // `*=`
  DEV_ASSIGN,         // `/=`
  MOD_ASSIGN,         // `%=`
  SHIFT_LEFT_ASSIGN,  // `<<=`
  SHIFT_RIGHT_ASSIGN, // `>>=`
  BIT_AND_ASSIGN,     // `&=`
  BIT_OR_ASSIGN,      // `|=`
  BIT_XOR_ASSIGN,     // `^=`
  // Misc operator
  ANDPERCENT, // `&`
  STAR,       // `*`
  COLN,       // `:`
  WHAT,       // `?`
} TokenKind;

typedef struct {
  // Value
  TokenKind kind;
  const char *value;
  // Position
  size_t line;
  size_t col;
} Token;

#endif // TRIDENT_TOKEN_H
