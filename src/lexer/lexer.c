#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeri.h"
#include "util/util.h"

#define TOKEN_CAP 1024

Lexer *init_lexer(const char *buffer) {
  static Lexer l = (Lexer){0};
  l.buffer = buffer;
  l.line = 1;
  l.col = 1;
  l.index = 0;
  l.token_pool_head = shi_opa_init(Token, TOKEN_CAP);
  l.token_pool = l.token_pool_head;
  l.map = shi_hs_init(TOKEN_CAP);
  return &l;
}

void lexer(Lexer *l, size_t len, Error *e) {
  while (l->index < len) {
    char c = peak(l, 0);
    size_t t_col = l->col;

    // Count lines
    if (isspace(c)) {
      if (c == '\n') {
        l->col = 1;
        ++l->line;
      } else {
        ++l->col;
      }
      consume(l);
      continue;
    }

    // Collect Identifiers
    if (isalpha(c) || c == '_') {
      size_t start = l->index;
      char *word = NULL;
      char *temp_word = get_word(l);
      TokenKind kind = get_keyword_kind(temp_word, l->index - start);

      if (kind == IDENTIFIER) {
        char *existing = shi_hs_get(l->map, temp_word);
        if (existing) {
          free(temp_word);
          word = existing;
        } else {
          shi_hs_put(l->map, temp_word, temp_word);
          word = temp_word;
        }
      } else {
        free(temp_word);
      }

      add_token(l, (Token){kind, word, l->line, t_col});
      continue;
    }

    // Collect Strings
    if (c == '\'' || c == '"') {
      char *str = NULL;
      TokenKind kind = c == '"' ? STRING : CHARACTER;
      char *temp_str = get_string(l, e, c);
      char *existing = shi_hs_get(l->map, temp_str);
      if (existing) {
        free(temp_str);
        str = existing;
      } else {
        shi_hs_put(l->map, temp_str, temp_str);
        str = temp_str;
      }
      add_token(l, (Token){kind, str, l->line, t_col});
      continue;
    }

    // Handle digits.
    // TODO: convert digits to int.
    if (isdigit(peak(l, 0)) || (peak(l, 0) == '.' && isdigit(peak(l, 1)))) {
      TokenKind kind = UNKNOWN;
      char *digit = get_digit(l, &kind);
      add_token(l, (Token){kind, digit, l->line, l->col});
      continue;
    }

    // Collect Directive & StringIdent
    if (c == '@' && (l->index + 1 < len) && (isalpha(peak(l, 1)) || peak(l, 1) == '"')) {
      consume(l);
      if (peak(l, 0) == '"') {
        char *str = NULL;
        char *temp_str = get_string_ident(l, e);
        char *existing = shi_hs_get(l->map, temp_str);
        if (existing) {
          free(temp_str);
          str = existing;
        } else {
          shi_hs_put(l->map, temp_str, temp_str);
          str = temp_str;
        }
        add_token(l, (Token){IDENTIFIER, str, l->line, t_col});
      } else {
        char *directive = get_word(l);
        TokenKind kind = get_directive_kind(directive);
        free(directive);
        if (kind == UNKNOWN) {
          add_error(e, (ErrorT){UNKNOWN_DIRECTIVE, l->line, l->col});
          continue;
        }
        add_token(l, (Token){kind, NULL, l->line, t_col});
      }
      continue;
    }

    // Skip Comments
    if (c == '/' && l->index + 1 < len && peak(l, 1) == '/') {
      while (l->index < len && peak(l, 0) != '\n') {
        consume(l);
      }
      continue;
    }

    // Handle operators and separators
    if (issymbol(c)) {
      TokenKind kind = get_symbol(l, len);
      if (kind != UNKNOWN) {
        add_token(l, (Token){kind, NULL, l->line, t_col});
        continue;
      }
      add_error(e, (ErrorT){UNKNOWN_TOKEN, l->line, t_col});
      continue;
    }
  }
  add_token(l, (Token){END_OF_TOKEN, NULL, 0, 0});
  add_error(e, (ErrorT){END_OF_ERROR, 0, 0});
}

void print_tokens(Lexer *l) {
  for (size_t i = 0;; ++i) {
    Token *token = (Token *)shi_opa_index(l->token_pool_head, i);
    if (!token || token->kind == END_OF_TOKEN) {
      break;
    }
    const char *kind = token_kind_to_str(token->kind);
    printf("%*s : %s\n", 18, kind, token->value ? token->value : "");
  }
}

void free_tokens(Lexer *l) {
  // TODO: convert digits to int or float and remove this for loop.
  for (size_t i = 0;; ++i) {
    Token *token = (Token *)shi_opa_index(l->token_pool_head, i);
    if (!token || token->kind == END_OF_TOKEN) {
      break;
    }
    if (token->kind == INT || token->kind == FLOAT) {
      free((char *)token->value);
    }
  }
  shi_hs_free_all(l->map, free);
  shi_opa_free(l->token_pool_head);
}
