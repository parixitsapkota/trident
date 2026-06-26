#include <stdio.h>

#include "ast.h"
#include "lexer/lexeri.h"
#include "parser.h"
#include "parseri.h"
#include "shi_opa.h"

#define AST_CAP 1024

Parser *init_parser(Lexer *l) {
  static Parser p = {0};
  p.ast = shi_arena_init(AST_CAP);
  p.tokens_pool = l->token_pool_head;
  return &p;
}

AstNode *parse_expr(Parser *p, Precedence prec);
AstNode *parse_left_f(Parser *p);

AstNode *parse_atom_f(Parser *p) {
  Token *tok = consume_p(p);
  if (is_kind_literal(tok->kind)) {
    AstNode *node = shi_arena_alloc(p->ast, sizeof(AstNode));

    *node = (AstNode){AST_ATOM, .atom = (AstAtom){tok->value, tok->kind}, tok->line, tok->col};
    return node;
  }

  return NULL;
}

AstNode *parse_unary_f(Parser *p) {
  Token *tok = consume_p(p);
  if (is_kind_prefix(tok->kind)) {
    AstNode *node = parse_left_f(p);

    AstNode *unary = shi_arena_alloc(p->ast, sizeof(AstNode));

    *unary = (AstNode){AST_UNARY, .unary = (AstUnary){.node = node, .op = tok->kind}, tok->line, tok->col};
    return unary;
  }

  return NULL;
}

AstNode *parse_left_f(Parser *p) {
  if (peak_kind(p, 0) == O_PREN) {
    consume_p(p);
    AstNode *node = parse_expr(p, PREC_NONE);
    // expect a `)`
    consume_p(p);
    return node;
  }

  if (is_kind_prefix(peak_kind(p, 0))) {
    return parse_unary_f(p);
  }

  return parse_atom_f(p);
}

AstNode *parse_expr(Parser *p, Precedence prec) {
  // TODO: error expest a `;`
  if (peak_kind(p, 0) == END_OF_TOKEN) {
    return NULL;
    // this return is a un handled error
  }

  AstNode *left = parse_left_f(p);

  while (1) {
    TokenKind op = peak_kind(p, 0);
    Precedence op_prec = get_op_prec(op);

    // prec_unknown is a error.
    if (op_prec == PREC_NONE || op_prec == PREC_UNKNOWN || op_prec < prec) {
      break;
    }

    Token *op_tok = consume_p(p);
    AstNode *right;
    if (is_proc_left_Associative(op_prec)) {
      right = parse_expr(p, op_prec + 1);
    } else {
      right = parse_expr(p, op_prec);
    }

    AstNode *node = shi_arena_alloc(p->ast, sizeof(AstNode));

    *node = (AstNode){AST_BINARY, .binary = (AstBinary){left, op, right}, op_tok->line, op_tok->col};
    left = node;
  }
  // expect a `;`
  return left;
}

void print_expr(AstNode *node) {
  if (!node) {
    return;
  }
  if (node->kind == AST_ATOM) {
    printf("%s\n", node->atom.value);
  } else if (node->kind == AST_BINARY) {
    print_expr(node->binary.left);
    print_expr(node->binary.right);
    printf("%s\n", token_kind_to_str(node->binary.op));
  } else if (node->kind == AST_UNARY) {
    print_expr(node->unary.node);
    printf("%s\n", token_kind_to_str(node->unary.op));
  }
}

void parse(Parser *p) {
  while (peak_kind(p, 0) != END_OF_TOKEN) {
    AstNode *root_node = parse_expr(p, PREC_NONE);
    print_expr(root_node);
    printf("\n");
  }
}

void free_parser(Parser *p) { shi_arena_free(p->ast); }
