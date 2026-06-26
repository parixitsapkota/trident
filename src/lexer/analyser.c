#include <ctype.h>

#include "error/error.h"
#include "lexeri.h"
#include "util/util.h"

// ==================
// Lexer Cursor
// ==================
char peak(const Lexer *l, const int offset) { return l->buffer[l->index + offset]; }
void consume(Lexer *l) { ++l->index; }
void add_token(Lexer *l, Token t) { shi_opa_push(l->token_pool, t); }

// ==================
// Get Identifiers & Keywords
// ==================
char *get_word(Lexer *l) {
  const size_t start = l->index;
  while (isalnum(peak(l, 0)) || peak(l, 0) == '_') {
    consume(l);
  }
  return substr(l->buffer, start, l->index);
}

// ==================
// Get Identifier
// ==================
char *get_string_ident(Lexer *l, Error *e) {
  consume(l); // Skip char `"`.
  const size_t start = l->index;

  while (peak(l, 0) != '"') {
    if (peak(l, 0) == '\0' || peak(l, 0) == '\n') {
      add_error(e, (ErrorT){.kind = UNTERMINATED_STRING, .line = l->line, .col = l->col});
      break;
    }
    consume(l);
  }
  if (peak(l, 0) == '"') {
    consume(l); // Skip char `"`.
  }
  return substr(l->buffer, start, l->index - 1);
}

// ==================
// Get String & Character
// ==================
// TODO: add support for escaping like `\n`,`\0`.
char *get_string(Lexer *l, Error *e, char c) {
  consume(l); // Skip first char.
  const size_t start = l->index;

  while (peak(l, 0) != c) {
    if (peak(l, 0) == '\0' || peak(l, 0) == '\n') {
      add_error(e, (ErrorT){.kind = UNTERMINATED_STRING, .line = l->line, .col = l->col});
      break;
    }
    consume(l);
  }
  if (peak(l, 0) == c) {
    consume(l); // Skip last char.
  }
  return substr(l->buffer, start, l->index - 1);
}

// TODO: add errors for incorrect formating
// (eg. 0_1784) : incorrect.
//          ^~
// ==================
// Get Digit
// ==================
static bool is_octal_digit(char c) { return c >= '0' && c <= '7'; }

static bool is_hex_digit(char c) { return isdigit((unsigned char)c) || (c >= 'A' && c <= 'F'); }

static char *get_binary(Lexer *l, TokenKind *kind) {
  size_t start = l->index;
  consume(l); // '0'
  consume(l); // 'b'
  while (peak(l, 0) == '0' || peak(l, 0) == '1') {
    consume(l);
  }
  *kind = INT;
  return substr(l->buffer, start, l->index);
}

static char *get_decimal(Lexer *l, TokenKind *kind) {
  size_t start = l->index;
  *kind = INT;
  while (isdigit((unsigned char)peak(l, 0))) {
    consume(l);
  }

  if (peak(l, 0) == '.') {
    *kind = FLOAT;
    consume(l); // consume '.'

    while (isdigit((unsigned char)peak(l, 0))) {
      consume(l);
    }

    if (peak(l, 0) == 'p') {
      consume(l); // 'p'
      if (peak(l, 0) == '+' || peak(l, 0) == '-') {
        consume(l);
      }
      while (is_octal_digit(peak(l, 0))) {
        consume(l);
      }
    }
  }
  return substr(l->buffer, start, l->index);
}

void get_prefix(Lexer *l, bool *has_dot, bool (*func)(char)) {
  consume(l); // '0'
  consume(l); // 'x`,`_'

  while (func(peak(l, 0))) {
    consume(l);
  }

  *has_dot = false;
  if (peak(l, 0) == '.') {
    *has_dot = true;
    consume(l);
    while (func(peak(l, 0))) {
      consume(l);
    }
  }
}

void get_suffix(Lexer *l, TokenKind *kind, bool has_dot, bool (*func)(char)) {
  if (peak(l, 0) == 'p') {
    *kind = FLOAT;
    consume(l); // 'p'
    if (peak(l, 0) == '+' || peak(l, 0) == '-') {
      consume(l);
    }
    while (func(peak(l, 0))) {
      consume(l);
    }
  } else if (has_dot) {
    *kind = FLOAT;
  } else {
    *kind = INT;
    if (peak(l, 0) == 'S') {
      consume(l);
    }
  }
}

static char *get_num(Lexer *l, TokenKind *kind, bool (*func)(char)) {
  size_t start = l->index;

  bool has_dot = false;
  get_prefix(l, &has_dot, func);

  get_suffix(l, kind, has_dot, func);
  return substr(l->buffer, start, l->index);
}

char *get_digit(Lexer *l, TokenKind *kind) {
  if (peak(l, 0) == '0' && peak(l, 1) == 'b') {
    return get_binary(l, kind);
  }
  if (peak(l, 0) == '0' && peak(l, 1) == '_') {
    return get_num(l, kind, is_octal_digit);
  }
  if (peak(l, 0) == '0' && peak(l, 1) == 'x') {
    return get_num(l, kind, is_hex_digit);
  }
  return get_decimal(l, kind);
}

// ==================
// Get Symbol
// ==================
TokenKind get_symbol(Lexer *l, size_t len) {

#define add_s(a, b)                                                                                                            \
  case a: consume(l); return b

#define add_2s(a, b, c, d)                                                                                                     \
  case a:                                                                                                                      \
    if (l->index + 1 < len && peak(l, 1) == b) {                                                                               \
      consume(l);                                                                                                              \
      consume(l);                                                                                                              \
      return d;                                                                                                                \
    } else {                                                                                                                   \
      consume(l);                                                                                                              \
      return c;                                                                                                                \
    }

#define add_2sc(a, b, c, d, e)                                                                                                 \
  case a:                                                                                                                      \
    if (l->index + 1 < len && peak(l, 1) == b) {                                                                               \
      consume(l);                                                                                                              \
      consume(l);                                                                                                              \
      return d;                                                                                                                \
    } else if (l->index + 1 < len && peak(l, 1) == a) {                                                                        \
      consume(l);                                                                                                              \
      consume(l);                                                                                                              \
      return e;                                                                                                                \
    } else {                                                                                                                   \
      consume(l);                                                                                                              \
      return c;                                                                                                                \
    }

#define add_3sc(a, b, c, d, e, f)                                                                                              \
  case a:                                                                                                                      \
    if (l->index + 1 < len && peak(l, 1) == b) {                                                                               \
      consume(l);                                                                                                              \
      consume(l);                                                                                                              \
      return d;                                                                                                                \
    } else if (l->index + 1 < len && peak(l, 1) == a) {                                                                        \
      if (l->index + 2 < len && peak(l, 2) == b) {                                                                             \
        consume(l);                                                                                                            \
        consume(l);                                                                                                            \
        consume(l);                                                                                                            \
        return f;                                                                                                              \
      } else {                                                                                                                 \
        consume(l);                                                                                                            \
        consume(l);                                                                                                            \
        return e;                                                                                                              \
      }                                                                                                                        \
    } else {                                                                                                                   \
      consume(l);                                                                                                              \
      return c;                                                                                                                \
    }

  switch (peak(l, 0)) {
    // Handle operators with 1 char.
    add_s('{', O_BRACE);
    add_s('}', C_BRACE);
    add_s('[', O_BRACKET);
    add_s(']', C_BRACKET);
    add_s('(', O_PREN);
    add_s(')', C_PREN);
    add_s(',', COMMA);
    add_s(':', COLN);
    add_s(';', SEMICOLON);
    add_s('~', BIT_NOT);
    add_s('?', WHAT);
    // Handle operators with 2 char.
    add_2s('=', '=', ASSIGN, EQUAL);
    add_2s('*', '=', STAR, MUL_ASSIGN);
    add_2s('%', '=', MOD, MOD_ASSIGN);
    add_2s('/', '=', DEV, DEV_ASSIGN);
    add_2s('!', '=', NEGATE, NOT_EQUAL);
    add_2s('^', '=', BIT_XOR, BIT_XOR_ASSIGN);
    add_2s('.', '.', ACESS, INCLUSIVE_RANGE);
    // Handle operators with 2 char, 3 cases.
    add_2sc('+', '=', ADD, ADD_ASSIGN, INCREMENT);
    add_2sc('-', '=', SUB, SUB_ASSIGN, DECREMENT);
    add_2sc('&', '=', ANDPERCENT, BIT_AND_ASSIGN, LOGICAL_AND);
    add_2sc('|', '=', BIT_OR, BIT_OR_ASSIGN, LOGICAL_OR);
    // Handle operators with 3 char, 3 cases.
    add_3sc('<', '=', LESSER, LESSER_EQUAL, SHIFT_LEFT, SHIFT_LEFT_ASSIGN);
    add_3sc('>', '=', GREATER, GREATER_EQUAL, SHIFT_RIGHT, SHIFT_RIGHT_ASSIGN);
  default: consume(l); return UNKNOWN;
  }
}

// ==================
// Get Kind String
// ==================
#define TOK_STR(x, y)                                                                                                          \
  case x: return y
char *token_kind_to_str(TokenKind kind) {
  switch (kind) {
    // Misc Tokens
    TOK_STR(UNKNOWN, "unknown");
    TOK_STR(END_OF_TOKEN, "");

    // Keywords
    // Qualifier
    TOK_STR(PUB, "pub");
    TOK_STR(EXTERN, "extern");
    TOK_STR(INLINE, "inline");
    TOK_STR(STATIC, "static");
    TOK_STR(REGISTER, "reister");
    TOK_STR(CONST, "const");
    TOK_STR(VAR, "var");
    TOK_STR(VOLATILE, "volatile");
    // Function
    TOK_STR(FUNC, "fn");
    TOK_STR(RET, "return");
    // Control flow
    TOK_STR(IF, "if");
    TOK_STR(ELSE, "else");
    TOK_STR(LOOP, "loop");
    TOK_STR(DO, "do");
    TOK_STR(WHILE, "while");
    TOK_STR(FOR, "for");
    TOK_STR(BREAK, "break");
    TOK_STR(CONTINUE, "continue");
    TOK_STR(SWITCH, "switch");
    TOK_STR(CASE, "case");
    TOK_STR(DEFAULT, "default");

    // Types
    TOK_STR(TYPE, "type");
    TOK_STR(VOID, "void");
    // Boolean
    TOK_STR(BOOL, "bool");
    TOK_STR(TRUE, "true");
    TOK_STR(FALSE, "flase");
    // Signed_Int
    TOK_STR(I8, "i8");
    TOK_STR(I16, "i16");
    TOK_STR(I32, "i32");
    TOK_STR(I64, "i64");
    TOK_STR(I128, "i128");
    TOK_STR(ISIZE, "isize");
    // Unsigned_Int
    TOK_STR(U8, "u8");
    TOK_STR(U16, "u16");
    TOK_STR(U32, "u32");
    TOK_STR(U64, "u64");
    TOK_STR(U128, "u128");
    TOK_STR(USIZE, "usize");
    // Float
    TOK_STR(F32, "f32");
    TOK_STR(F64, "f64");
    TOK_STR(F128, "f128");
    // Struct And Union
    TOK_STR(STRUCT, "struct");
    TOK_STR(UNION, "union");
    // Enumerator
    TOK_STR(ENUM, "enum");
    TOK_STR(ERROR, "err");
    // Identifier & Literals
    TOK_STR(IDENTIFIER, "identfier::");
    TOK_STR(INT, "int::");
    TOK_STR(FLOAT, "float::");
    TOK_STR(STRING, "string::");
    TOK_STR(CHARACTER, "char");

    // Directive
    TOK_STR(TYPEOF, "@typedef");
    TOK_STR(DEFINE, "@define");
    TOK_STR(IMPORT, "@import");

    // Seperator
    TOK_STR(O_BRACE, "{");
    TOK_STR(C_BRACE, "}");
    TOK_STR(O_PREN, "(");
    TOK_STR(C_PREN, ")");
    TOK_STR(O_BRACKET, "[");
    TOK_STR(C_BRACKET, "]");
    TOK_STR(SEMICOLON, ";");
    // Operator
    TOK_STR(COMMA, ",");
    TOK_STR(ACESS, ".");
    TOK_STR(ADD, "+");
    TOK_STR(SUB, "-");
    TOK_STR(DEV, "/");
    TOK_STR(MOD, "%");
    // Range
    TOK_STR(INCLUSIVE_RANGE, "..");

    // Unary
    TOK_STR(INCREMENT, "++");
    TOK_STR(DECREMENT, "--");
    TOK_STR(NEGATE, "!");
    // Logical
    TOK_STR(LOGICAL_AND, "&&");
    TOK_STR(LOGICAL_OR, "||");
    TOK_STR(LOGICAL_XOR, "^^");
    // Bitwise Logic
    TOK_STR(BIT_NOT, "~");
    TOK_STR(BIT_OR, "|");
    TOK_STR(BIT_XOR, "^");
    // Shift
    TOK_STR(SHIFT_LEFT, "<<");
    TOK_STR(SHIFT_RIGHT, ">>");
    // Compare
    TOK_STR(EQUAL, "==");
    TOK_STR(NOT_EQUAL, "!=");
    TOK_STR(LESSER, "<");
    TOK_STR(GREATER, ">");
    TOK_STR(LESSER_EQUAL, "<=");
    TOK_STR(GREATER_EQUAL, ">=");
    // Assignment
    TOK_STR(ASSIGN, "=");
    TOK_STR(ADD_ASSIGN, "+=");
    TOK_STR(SUB_ASSIGN, "-=");
    TOK_STR(MUL_ASSIGN, "*=");
    TOK_STR(DEV_ASSIGN, "/=");
    TOK_STR(MOD_ASSIGN, "%=");
    TOK_STR(SHIFT_LEFT_ASSIGN, "<<=");
    TOK_STR(SHIFT_RIGHT_ASSIGN, ">>=");
    TOK_STR(BIT_AND_ASSIGN, "&=");
    TOK_STR(BIT_OR_ASSIGN, "|=");
    TOK_STR(BIT_XOR_ASSIGN, "^=");
    // Misc operator
    TOK_STR(ANDPERCENT, "&");
    TOK_STR(STAR, "*");
    TOK_STR(COLN, ":");
    TOK_STR(WHAT, "?");

  default: return "";
  }
}
