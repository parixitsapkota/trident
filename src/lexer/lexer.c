#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../SHI/shi_opa.h"
#include "lexer.h"

void free_token_pool(SHI_OPA *token_pool) {
  Token *token = shi_opa_index(token_pool, 0);
  for (size_t i = 0; token->kind == END_OF_TOKEN; ++i) {
    token = shi_opa_index(token_pool, i);
    if (!token->value) {
      free(token->value);
    }
  }
  free_mem_chain(token_pool);
}

#define TOK_STR(x)                                                             \
  case x:                                                                      \
    return #x
char *token_kind_to_str(TokenKind kind) {
  switch (kind) {

    // Qualifier
    TOK_STR(PUB);
    TOK_STR(EXTERN);
    TOK_STR(INLINE);
    TOK_STR(STATIC);
    TOK_STR(REGISTER);
    TOK_STR(CONST);
    TOK_STR(VAR);
    TOK_STR(VOLATILE);

    // Function
    TOK_STR(FUNC);
    TOK_STR(RET);

    // Control flow
    TOK_STR(IF);
    TOK_STR(ELSE);
    TOK_STR(LOOP);
    TOK_STR(DO);
    TOK_STR(WHILE);
    TOK_STR(FOR);
    TOK_STR(BREAK);
    TOK_STR(CONTINUE);
    TOK_STR(SWITCH);
    TOK_STR(CASE);
    TOK_STR(DEFAULT);

    // Types
    TOK_STR(TYPE);
    TOK_STR(VOID);

    // Boolean
    TOK_STR(BOOL);
    TOK_STR(TRUE);
    TOK_STR(FALSE);

    // Signed_Int
    TOK_STR(I8);
    TOK_STR(I16);
    TOK_STR(I32);
    TOK_STR(I64);
    TOK_STR(I128);
    TOK_STR(ISIZE);

    // Unsigned_Int
    TOK_STR(U8);
    TOK_STR(U16);
    TOK_STR(U32);
    TOK_STR(U64);
    TOK_STR(U128);
    TOK_STR(USIZE);

    // Float
    TOK_STR(F32);
    TOK_STR(F64);
    TOK_STR(F128);

    // Struct And Union
    TOK_STR(STRUCT);
    TOK_STR(UNION);

    // Enumerator
    TOK_STR(ENUM);
    TOK_STR(ERROR);

    // Identifier & Literals
    TOK_STR(IDENTIFIER);
    TOK_STR(INT);
    TOK_STR(FLOAT);
    TOK_STR(STRING);
    TOK_STR(CHARACTER);

    // Directive
    TOK_STR(TYPEOF);
    TOK_STR(DEFINE);
    TOK_STR(IMPORT);

    // Seperator
    TOK_STR(O_BRACE);
    TOK_STR(C_BRACE);
    TOK_STR(O_PREN);
    TOK_STR(C_PREN);
    TOK_STR(O_BRACKET);
    TOK_STR(C_BRACKET);
    TOK_STR(SEMICOLON);
    TOK_STR(COLN);
    TOK_STR(WHAT);

    // Operator
    TOK_STR(COMMA);
    TOK_STR(DOT);
    TOK_STR(DOT_DOT);
    TOK_STR(PLUS);
    TOK_STR(DASH);
    TOK_STR(STAR);
    TOK_STR(SLASH);
    TOK_STR(PERCENT);

    // Unary
    TOK_STR(INCREMENT);
    TOK_STR(DECREMENT);
    TOK_STR(NEGATE);

    // Logical
    TOK_STR(LOGICAL_AND);
    TOK_STR(LOGICAL_OR);
    TOK_STR(LOGICAL_XOR);

    // Bitwise Logic
    TOK_STR(BITWISE_NOT);
    TOK_STR(ANDPERCENT);
    TOK_STR(BITWISE_OR);
    TOK_STR(BITWISE_XOR);

    // Shift
    TOK_STR(SHIFT_LEFT);
    TOK_STR(SHIFT_RIGHT);

    // Compare
    TOK_STR(EQUAL);
    TOK_STR(NOT_EQUAL);
    TOK_STR(LESSER);
    TOK_STR(GREATER);
    TOK_STR(LESSER_EQUAL);
    TOK_STR(GREATER_EQUAL);

    // Assignment
    TOK_STR(ASSIGN);
    TOK_STR(PLUS_ASSIGN);
    TOK_STR(DASH_ASSIGN);
    TOK_STR(STAR_ASSIGN);
    TOK_STR(SLASH_ASSIGN);
    TOK_STR(PERCENT_ASSIGN);
    TOK_STR(SHIFT_LEFT_ASSIGN);
    TOK_STR(SHIFT_RIGHT_ASSIGN);
    TOK_STR(AND_ASSIGN);
    TOK_STR(OR_ASSIGN);
    TOK_STR(XOR_ASSIGN);

    // Error
    TOK_STR(UNTERMINATED_STRING);
    TOK_STR(UNTERMINATED_CHAR);
    TOK_STR(UNKNOWN_TOKEN);
    TOK_STR(UNKNOWN_DIRECTIVE);

    // End of Tokens & unknown
    TOK_STR(END_OF_TOKEN);
    TOK_STR(UNKNOWN);

  default:
    return "UNKNOWN_TOKEN_KIND";
  }
}

static int compare_keywords(const void *a, const void *b) {
  const char *search_word = a;
  const KeywordMap *map_item = b;
  return strcmp(search_word, map_item->word);
}

TokenKind get_keyword_kind(const char *word) {
  if (!word || *word == '\0') {
    return IDENTIFIER;
  }
  const KeywordMap *match =
      bsearch(word, keywords, sizeof(keywords) / sizeof(KeywordMap),
              sizeof(KeywordMap), compare_keywords);
  return match ? match->kind : IDENTIFIER;
}

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

bool is_token_kind_error(const TokenKind kind) {
  switch (kind) {
  case UNKNOWN:
  case UNTERMINATED_STRING:
  case UNTERMINATED_CHAR:
  case UNKNOWN_TOKEN:
  case UNKNOWN_DIRECTIVE:
    return 1;
  default:
    return 0;
  }
}

size_t print_error_token_kind(const char *file_name, SHI_OPA *token_pool) {
  size_t errors = 0;
  for (size_t i = 0;; ++i) {
    Token *token = shi_opa_index(token_pool, i);
    if (is_token_kind_error(token->kind)) {
      ++errors;
      fprintf(stderr, "%s:%lu:%lu: %s", file_name, token->line, token->col,
              token_kind_to_str(token->kind));
      if (token->value) {
        fprintf(stderr, "\t%s", token->value);
      }
      fputc('\n', stderr);
      continue;
    }
    if (token->kind == END_OF_TOKEN)
      break;
  }
  if (errors)
    fprintf(stderr, "%lu ERRORS!\n", errors);
  return errors;
}

void print_tokens(SHI_OPA *token_pool) {
  for (size_t i = 0;; ++i) {
    Token *token = shi_opa_index(token_pool, i);

    const char *kind = token_kind_to_str(token->kind);
    printf("%-18s :%lu:%lu:\t%s\n", kind, token->line, token->col,
           token->value);

    if (token->kind == END_OF_TOKEN)
      break;
  }
}

char *substr(const char *str, const size_t start, const size_t end) {
  const size_t length = end - start;
  char *substr = malloc(length + 1);
  strncpy(substr, str + start, length);
  substr[length] = '\0';
  return substr;
}

char *get_word(const char *buffer, size_t *i, size_t *col) {
  const size_t start = *i;
  while (isalnum(buffer[*i]) || buffer[*i] == '_') {
    ++*col;
    ++*i;
  }
  return substr(buffer, start, *i);
}

char *get_digit(const char *buffer, size_t *i, int *is_float, size_t *col) {
  const size_t start = *i;
  *is_float = 0;
  if (buffer[*i] == '.') {
    *is_float = 1;
    ++*col;
    ++*i;
  }
  while (isdigit(buffer[*i]) || buffer[*i] == '.') {
    if (buffer[*i] == '.') {
      if (*is_float) {
        break;
      }
      *is_float = 1;
    }
    ++*col;
    ++*i;
  }
  return substr(buffer, start, *i);
}

char *get_string(const char *buffer, size_t *i, size_t *col, TokenKind *error) {
  const size_t start = ++*i; // Skip char `"`.
  ++*col;
  while (buffer[*i] != '"') {
    if (buffer[*i] == '\0' || buffer[*i] == '\n') {
      *error = UNTERMINATED_STRING;
      break;
    }
    ++*col;
    ++*i;
  }
  if (buffer[*i] == '"') {
    ++*i; // Skip char `"`.
    ++*col;
  }
  return substr(buffer, start, *i - 1);
}

char *get_string_ident(const char *buffer, size_t *i, size_t *col,
                       TokenKind *error) {
  const size_t start = ++*i; // Skip char `"`.
  ++*col;
  while (buffer[*i] != '"') {
    if (buffer[*i] == '\0' || buffer[*i] == '\n') {
      *error = UNTERMINATED_STRING;
      break;
    }
    ++*col;
    ++*i;
  }
  if (buffer[*i] == '"') {
    ++*i; // Skip char `"`.
    ++*col;
  }
  return substr(buffer, start, *i - 1);
}

char *get_char(const char *buffer, size_t *i, size_t *col, TokenKind *error) {
  const size_t start = ++*i; // Skip char `"`.
  ++*col;
  while (buffer[*i] != '\'') {
    if (buffer[*i] == '\0' || buffer[*i] == '\n') {
      *error = UNTERMINATED_CHAR;
      break;
    }
    ++*col;
    ++*i;
  }
  if (buffer[*i] == '\'') {
    ++*i; // Skip char `'`.
    ++*col;
  }
  return substr(buffer, start, *i - 1);
}

SHI_OPA *lexer(const char *buffer) {

  SHI_OPA *token_pool_head = shi_opa_init(Token, 1024);
  SHI_OPA *token_pool = token_pool_head;

  size_t line = 1, col = 1;
  size_t i = 0;
  Token c_token = {0};

  while (buffer[i] != '\0') {

    // Skip line comment
    if (buffer[i] == '/' && buffer[i + 1] == '/') {
      while (buffer[i] != '\n') {
        if (buffer[i] == '\0') {
          break;
        }
        ++col;
        ++i;
      }
      continue;
    }

    // Handle whitespace.
    if (isspace(buffer[i])) {
      if (buffer[i] == '\n') {
        col = 1;
        ++line;
      } else {
        ++col;
      }
      ++i;
      continue;
    }

    const size_t tcol = col;

    // Handle identifiers & keywords.
    if (isalpha(buffer[i]) || buffer[i] == '_') {
      char *word = get_word(buffer, &i, &col);
      const TokenKind kind = get_keyword_kind(word);
      if (kind != IDENTIFIER) {
        free(word);
        c_token = (Token){kind, NULL, line, tcol};
      } else {
        c_token = (Token){kind, word, line, tcol};
      }
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle digits.
    if (isdigit(buffer[i]) || (buffer[i] == '.' && isdigit(buffer[i + 1]))) {
      int is_float = 0;
      const char *digit = get_digit(buffer, &i, &is_float, &col);
      const TokenKind kind = is_float ? FLOAT : INT;
      c_token = (Token){kind, (char *)digit, line, tcol};
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle strings.
    if (buffer[i] == '"') {
      TokenKind error = STRING;
      const char *string = get_string(buffer, &i, &col, &error);
      c_token = (Token){error, (char *)string, line, tcol};
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle character.
    if (buffer[i] == '\'') {
      TokenKind error = CHARACTER;
      const char *char_ = get_char(buffer, &i, &col, &error);
      c_token = (Token){error, (char *)char_, line, tcol};
      shi_opa_push(token_pool, c_token);
      continue;
    }

    // Handle directives.
    if (buffer[i] == '@') {
      if (buffer[i + 1] != '\0' && buffer[i + 1] == '"') {
        ++i; // Skip `@` char.
        ++col;
        TokenKind error = IDENTIFIER;
        const char *ident = get_string_ident(buffer, &i, &col, &error);
        c_token = (Token){error, (char *)ident, line, tcol};
        shi_opa_push(token_pool, c_token);
        continue;
      } else if (buffer[i + 1] != '\0' &&
                 isalpha((unsigned char)buffer[i + 1])) {
        ++i; // Skip `@` char.
        ++col;
        char *directive = get_word(buffer, &i, &col);
        TokenKind kind = get_directive_kind(directive);
        if (kind != UNKNOWN) {
          c_token = (Token){kind, (char *)directive, line, tcol};
        } else {
          free(directive);
          c_token = (Token){UNKNOWN_DIRECTIVE, NULL, line, tcol};
        }
        shi_opa_push(token_pool, c_token);
        continue;
      }
    }

// Handle operators and seperators.
#define incLC                                                                  \
  ++col;                                                                       \
  ++i

#define add_s(a, b)                                                            \
  case a:                                                                      \
    c_token = (Token){b, NULL, line, tcol};                                    \
    break

#define add_2s(a, b, c, d)                                                     \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      c_token = (Token){d, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else {                                                                   \
      c_token = (Token){c, NULL, line, tcol};                                  \
    }                                                                          \
    break

#define add_2sc(a, b, c, d, e)                                                 \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      c_token = (Token){d, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else if (buffer[i + 1] == a) {                                           \
      c_token = (Token){e, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else {                                                                   \
      c_token = (Token){c, NULL, line, tcol};                                  \
    }                                                                          \
    break

#define add_3sc(a, b, c, d, e, f)                                              \
  case a:                                                                      \
    if (buffer[i + 1] == b) {                                                  \
      c_token = (Token){d, NULL, line, tcol};                                  \
      incLC;                                                                   \
    } else if (buffer[i + 1] == a) {                                           \
      if (buffer[i + 2] == b) {                                                \
        c_token = (Token){f, NULL, line, tcol};                                \
        incLC;                                                                 \
      } else {                                                                 \
        c_token = (Token){e, NULL, line, tcol};                                \
      }                                                                        \
      incLC;                                                                   \
    } else {                                                                   \
      c_token = (Token){c, NULL, line, tcol};                                  \
    }                                                                          \
    break

    switch (buffer[i]) {
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
      add_s('~', BITWISE_NOT);
      add_s('?', WHAT);
      // Handle operators with 2 char.
      add_2s('*', '=', STAR, STAR_ASSIGN);
      add_2s('%', '=', PERCENT, PERCENT_ASSIGN);
      add_2s('/', '=', SLASH, SLASH_ASSIGN);
      add_2s('=', '=', ASSIGN, EQUAL);
      add_2s('!', '=', NEGATE, NOT_EQUAL);
      add_2s('^', '=', BITWISE_XOR, XOR_ASSIGN);
      add_2s('.', '.', DOT, DOT_DOT);
      // Handle operators with 2 char, 3 cases.
      add_2sc('+', '=', PLUS, PLUS_ASSIGN, INCREMENT);
      add_2sc('-', '=', DASH, DASH_ASSIGN, DECREMENT);
      add_2sc('&', '=', ANDPERCENT, AND_ASSIGN, LOGICAL_AND);
      add_2sc('|', '=', BITWISE_OR, OR_ASSIGN, LOGICAL_OR);
      // Handle operators with 3 char, 3 cases.
      add_3sc('<', '=', LESSER, LESSER_EQUAL, SHIFT_LEFT, SHIFT_LEFT_ASSIGN);
      add_3sc('>', '=', GREATER, GREATER_EQUAL, SHIFT_RIGHT,
              SHIFT_RIGHT_ASSIGN);
    default:
      c_token = (Token){UNKNOWN_TOKEN, NULL, line, tcol};
    }

    shi_opa_push(token_pool, c_token);
    incLC;
  }

  c_token = (Token){END_OF_TOKEN, NULL, 0, 0};
  shi_opa_push(token_pool, c_token);
  return token_pool_head;
}
