#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_interned_symbols(SHI_HS *set) {
  for (size_t i = 0; i < set->bucket_cap; ++i) {
    __hs_entry__ *entry = set->buckets[i];
    while (entry != NULL) {
      free(entry->value);
      entry = entry->next;
    }
  }
}

void free_token_pool(SHI_OPA *token_pool) {
  for (size_t i = 0;; ++i) {
    Token *token = (Token *)shi_opa_index(token_pool, i);
    if (!token || token->kind == END_OF_TOKEN)
      break;
    if (token->kind != IDENTIFIER && token->value != NULL) {
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
