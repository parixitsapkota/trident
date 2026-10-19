#include <stdio.h>

#include "token.h"

#include "core/arena.h"
#include "core/color.h"

const char *token_kind_to_str(TokenKind kind) {
    switch (kind) {
#define LITERAL(tok)                                                                     \
    case tok##_LIT: return #tok;
#define KEYWORD(tok, name)                                                               \
    case KW_##tok: return name;
#define PUNCTUATION(tok, name)                                                           \
    case PUNCT_##tok: return name;
#define TOKEN(tok, name)                                                                 \
    case tok: return name;
#include "token.def"
#undef TOKEN
#undef LITERAL
#undef KEYWORD
#undef PUNCTUATION
    default: return "\"nil\"";
    }
}

Token *token(Arena *arena, TokenKind kind, const char *lexeme, Span *span, NumValue val) {
    Token *tok = arena_alloc(arena, sizeof(Token));
    *tok = (Token){kind, lexeme, span, val, NULL, NULL};
    return tok;
}

void print_tokens(const char *file_name, const Token *tokens) {
    for (const Token *cur = tokens; cur != NULL; cur = cur->next) {
        fprintf(stderr,
                FG_CYAN "%s" RESET ":" FG_BLACK "%zu:%zu:\t" BOLD FG_YELLOW
                        "%-12s\t" RESET BOLD,
                file_name, cur->span->start.ln, cur->span->start.cn,
                token_kind_to_str(cur->kind));
        switch (cur->kind) {
        case CHARACTER_LIT: fprintf(stderr, FG_RED "%c", (char)cur->val.i); break;
        case INT_LIT: fprintf(stderr, FG_RED "%zu", cur->val.i); break;
        case FLOAT_LIT: fprintf(stderr, FG_RED "%g", cur->val.f); break;
        case STRING_LIT: fprintf(stderr, FG_GREEN "\"%s\"", cur->lexeme); break;
#define LITERAL(tok)
#define KEYWORD(tok, name)                                                               \
    case KW_##tok: fprintf(stderr, FG_MAGENTA "%s", cur->lexeme); break;
#define PUNCTUATION(tok, name)                                                           \
    case PUNCT_##tok: fprintf(stderr, FG_CYAN "%s", token_kind_to_str(cur->kind)); break;
#define TOKEN(tok, name)                                                                 \
    case tok: break;
#include "token.def"
#undef TOKEN
#undef LITERAL
#undef KEYWORD
#undef PUNCTUATION
        default:
            fprintf(stderr, FG_BLUE "%s",
                    cur->lexeme ? cur->lexeme : token_kind_to_str(cur->kind));
            break;
        }
        fprintf(stderr, RESET "\n");
    }
}

void print_tokens_reversed(const char *file_name, const Token *tokens) {
    Token *last_token = NULL;
    for (const Token *cur = tokens; cur != NULL; cur = cur->next) {
        if (cur->kind == END_OF_TOKEN) {
            last_token = (Token *)cur;
            break;
        }
    }
    for (const Token *cur = last_token; cur != NULL; cur = cur->last) {
        fprintf(stderr,
                FG_CYAN "%s" RESET ":" FG_BLACK "%zu:%zu:\t" BOLD FG_YELLOW
                        "%-12s\t" RESET BOLD,
                file_name, cur->span->start.ln, cur->span->start.cn,
                token_kind_to_str(cur->kind));
        switch (cur->kind) {
        case CHARACTER_LIT: fprintf(stderr, FG_RED "%c", (char)cur->val.i); break;
        case INT_LIT: fprintf(stderr, FG_RED "%zu", cur->val.i); break;
        case FLOAT_LIT: fprintf(stderr, FG_RED "%g", cur->val.f); break;
        case STRING_LIT: fprintf(stderr, FG_GREEN "\"%s\"", cur->lexeme); break;
#define LITERAL(tok)
#define KEYWORD(tok, name)                                                               \
    case KW_##tok: fprintf(stderr, FG_MAGENTA "%s", cur->lexeme); break;
#define PUNCTUATION(tok, name)                                                           \
    case PUNCT_##tok: fprintf(stderr, FG_CYAN "%s", token_kind_to_str(cur->kind)); break;
#define TOKEN(tok, name)                                                                 \
    case tok: break;
#include "token.def"
#undef TOKEN
#undef LITERAL
#undef KEYWORD
#undef PUNCTUATION
        default:
            fprintf(stderr, FG_BLUE "%s",
                    cur->lexeme ? cur->lexeme : token_kind_to_str(cur->kind));
            break;
        }
        fprintf(stderr, RESET "\n");
    }
}
