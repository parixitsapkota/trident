#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

#include "core/def.h"
#include "core/mem/mem.h"
#include "syntax/token/token.h"

static const char *ast_kind_to_string(AstKind kind) {
    static const char *const names[] = {
        [AST_PROGRAM] = "PROGRAM",

        /* expressions */
        [AST_IDENT] = "IDENT",
        [AST_INT] = "INT",
        [AST_FLOAT] = "FLOAT",
        [AST_STRING] = "STRING",
        [AST_CHAR] = "CHAR",
        [AST_BOOL] = "BOOL",
        [AST_UNARY] = "UNARY",
        [AST_BINARY] = "BINARY",
        [AST_INDEX] = "INDEX",
        [AST_CALL] = "CALL",

        /* statements */
        [AST_BLOCK] = "BLOCK",
        [AST_EXPR_STMT] = "EXPR_STMT",
        [AST_IF] = "IF",
        [AST_ELSE_IF] = "ELSE_IF",
        [AST_ELSE] = "ELSE",
        [AST_LOOP] = "LOOP",
        [AST_WHILE] = "WHILE",
        [AST_MATCH] = "MATCH",
        [AST_CASE] = "CASE",
        [AST_BREAK] = "BREAK",
        [AST_CONTINUE] = "CONTINUE",
        [AST_RETURN] = "RETURN",

        /* declarations */
        [AST_FUNCTION] = "FUNCTION",
        [AST_PARAM] = "PARAM",
        [AST_LET] = "LET",

        /* type */
        [AST_TYPE_NAME] = "TYPE_NAME",
        [AST_TYPE_POINTER] = "TYPE_POINTER",
        [AST_TYPE_SLICE] = "TYPE_SLICE",
        [AST_TYPE_ARRAY] = "TYPE_ARRAY",

        [AST_STRUCT] = "STRUCT",
        [AST_UNION] = "UNION",
    };

    size_t count = sizeof(names) / sizeof(names[0]);
    if ((size_t)kind < count && names[kind] != NULL) {
        return names[kind];
    }
    return "UNKNOWN";
}

static void print_node_label(const AstNode *node) {
    const char *kind = ast_kind_to_string(node->kind);

    switch (node->kind) {
    case AST_PROGRAM: printf(FGB_CYAN "%s:" RESET, kind); break;

    case AST_INT:
        printf(FGB_MAGENTA "%s: " FG_GREEN "\"%zu\"" RESET, kind, node->as.int_val);
        break;

    case AST_CHAR:
        printf(FGB_MAGENTA "%s: " FG_GREEN "%zu" RESET, kind, node->as.int_val);
        break;

    case AST_IDENT:
        printf(FGB_MAGENTA "%s: " FG_GREEN "\"%s\"" RESET, kind, node->as.ident.name);
        break;

    case AST_STRING:
        printf(FGB_MAGENTA "%s: " FG_GREEN "\"%s\"" RESET, kind, node->as.str);
        break;

    case AST_BOOL:
        printf(FGB_CYAN "%s: " FG_RED "%s" RESET, kind,
               node->as.bool_val ? "true" : "false");
        break;

    case AST_UNARY:
        printf(FGB_CYAN "%s: " FG_RED "%s" FGB_CYAN " : " FG_RED "`%s`" RESET, kind,
               node->as.unary.postfix ? "postfix" : "prefix",
               token_kind_to_str(node->as.unary.op));
        break;

    case AST_BINARY:
        printf(FGB_CYAN "%s:" FGB_CYAN " : " FG_RED "`%s`" RESET, kind,
               token_kind_to_str(node->as.binary.op));
        break;

    case AST_LET:
        printf(FGB_MAGENTA "%s:" FG_RED "%s" FG_GREEN " \"%s\" " RESET, kind,
               node->as.let.mutable ? "" : " mut", node->as.let.name);
        break;

    case AST_FUNCTION:
        printf(FG_RED "%s: " FG_BLUE "%s" FG_GREEN "\"%s\"" FG_RED " %s" RESET, kind,
               node->as.func.public ? "public " : "", node->as.func.name,
               node->as.func.noreturn ? "noreturn" : "");
        break;

    case AST_PARAM:
        printf(FGB_MAGENTA "%s: " FG_GREEN " \"%s\"" RESET, kind, node->as.param.name);
        break;

    case AST_CALL:
        printf(FGB_MAGENTA "%s: " FG_GREEN "\"%s\"" RESET, kind, node->as.call.callee);
        break;

    case AST_TYPE_NAME:
        printf(FGB_CYAN "%s: -> " FG_GREEN "\"%s\"" RESET, kind,
               node->as.type_tok->lexeme);
        break;

    case AST_TYPE_ARRAY: printf(FGB_CYAN "%s: " RESET, kind); break;

    case AST_TYPE_POINTER:
    case AST_TYPE_SLICE:
        printf(FGB_CYAN "%s: " FG_RED "%s" RESET, kind,
               node->as.pointer.mutable ? "mut" : "const");
        break;

    case AST_UNION:
    case AST_STRUCT:
        printf(FG_RED "%s: " FG_GREEN "\"%s\"" RESET, kind, node->as.structure.name);
        break;

    default: printf(FG_RED "%s:" RESET, kind); break;
    }
    putchar('\n');
}

typedef struct {
    AstNode *heads[4];
    size_t n;
} Kids;

static void kids_add(Kids *k, AstNode *head) {
    if (head) k->heads[k->n++] = head;
}

static Kids get_kids(const AstNode *n) {
    Kids k = {0};
    switch (n->kind) {
    case AST_UNARY: kids_add(&k, n->as.unary.operand); break;

    case AST_BINARY:
        kids_add(&k, n->as.binary.lhs);
        kids_add(&k, n->as.binary.rhs);
        break;

    case AST_IF:
        kids_add(&k, n->as.cond.cond);
        kids_add(&k, n->as.cond.then_b);
        kids_add(&k, n->as.cond.else_b);
        break;

    case AST_WHILE:
    case AST_CASE:
        kids_add(&k, n->as.ctl.expr);
        kids_add(&k, n->as.ctl.body);
        break;

    case AST_INDEX:
        kids_add(&k, n->as.index.left);
        kids_add(&k, n->as.index.index);
        break;

    case AST_CALL: kids_add(&k, n->as.call.args); break;

    case AST_PROGRAM:
    case AST_BLOCK:
    case AST_EXPR_STMT:
    case AST_CONTINUE:
    case AST_BREAK:
    case AST_RETURN:
    case AST_ELSE:
    case AST_LOOP: kids_add(&k, n->as.stmt.expr); break;

    case AST_LET:
        kids_add(&k, n->as.let.type);
        kids_add(&k, n->as.let.expr);
        break;

    case AST_TYPE_ARRAY:
        kids_add(&k, n->as.array.inner);
        kids_add(&k, n->as.array.len);
        break;

    case AST_TYPE_POINTER:
    case AST_TYPE_SLICE: kids_add(&k, n->as.pointer.inner); break;

    case AST_UNION:
    case AST_STRUCT: kids_add(&k, n->as.structure.body); break;

    case AST_FUNCTION:
        kids_add(&k, n->as.func.return_type);
        kids_add(&k, n->as.func.params);
        kids_add(&k, n->as.func.body);
        break;

    case AST_PARAM: kids_add(&k, n->as.param.type); break;

    default: break;
    }
    return k;
}

typedef struct {
    char *buf;
    size_t len, cap;
} Prefix;

static void prefix_push(Prefix *p, const char *s) {
    size_t n = strlen(s);
    if (p->len + n + 1 > p->cap) {
        p->cap = (p->len + n + 1) * 2;
        p->buf = xrealloc(p->buf, p->cap);
        if (!p->buf) abort();
    }
    memcpy(p->buf + p->len, s, n + 1);
    p->len += n;
}

static void prefix_pop(Prefix *p, size_t old_len) {
    p->len = old_len;
    p->buf[old_len] = '\0';
}

static void print_tree(const AstNode *node, Prefix *p, bool is_last);

static void print_children(const Kids *k, Prefix *p) {
    for (size_t i = 0; i < k->n; i++)
        for (const AstNode *c = k->heads[i]; c; c = c->next)
            print_tree(c, p, !c->next && i + 1 == k->n);
}

static void print_tree(const AstNode *node, Prefix *p, bool is_last) {
    fputs(p->buf, stdout);
    fputs(is_last ? "└── " : "├── ", stdout);
    print_node_label(node);

    size_t mark = p->len;
    prefix_push(p, is_last ? "    " : "│   ");
    Kids k = get_kids(node);
    print_children(&k, p);
    prefix_pop(p, mark);
}

void print_ast(AstNode *root) {
    static char outbuf[1 << 16];
    setvbuf(stdout, outbuf, _IOFBF, sizeof outbuf);

    Prefix p = {0};
    prefix_push(&p, "");

    for (const AstNode *n = root; n; n = n->next) {
        print_node_label(n);
        Kids k = get_kids(n);
        print_children(&k, &p);
    }

    fflush(stdout);
    free(p.buf);
}
