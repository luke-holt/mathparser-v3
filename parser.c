#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

typedef enum {
    ADD, SUB, MUL, DIV, POW,
    LPAREN, RPAREN, NUM, SPACE,
} TokenType;

typedef struct TreeNode TreeNode;
struct TreeNode {
    TokenType type;
    union {
        struct { TreeNode *left, *right; } binary;
        struct { TreeNode *arg; } unary;
        float number;
    };
};

typedef struct {
    TokenType type;
    float value;
} Token;

typedef struct {
    Token *items;
    size_t count, capacity, index;
} Tokens;

static Tokens *tokenize(const char *str);

static Tokens *tokens_new(void);
static void tokens_destroy(Tokens *list);
static void tokens_append(Tokens *list, Token token);
static const Token *tokens_peek(Tokens *list);
static const Token *tokens_next(Tokens *list);
static void tokens_print(Tokens *list);

static TreeNode *parse_addsub(Tokens *list);
static TreeNode *parse_muldiv(Tokens *list);
static TreeNode *parse_power(Tokens *list);
static TreeNode *parse_parens(Tokens *list);

static void tree_print(const TreeNode *root);
static void tree_print_recurse(const TreeNode *root);
static void tree_destroy(TreeNode *root);

float
mathparser_eval_expr(const char *expr)
{
    Tokens *list;
    list = tokenize(expr);
    tokens_print(list);

    TreeNode *tree;
    tree = parse_addsub(list);

    tree_print(tree);

    tokens_destroy(list);
    tree_destroy(tree);
    return 0.0;
}

Tokens *
tokenize(const char *str)
{
    Tokens *list;
    size_t len, strpos, i;

    len = strlen(str);
    list = tokens_new();

    strpos = i = 0;

    while (i < len) {
        Token t;

        switch (str[i]) {
        case '+': t.type = ADD; break;
        case '-': t.type = SUB; break;
        case '*':
            if ('*' == str[i+1]) { t.type = POW; i++; }
            else { t.type = MUL; }
            break;
        case '/': t.type = DIV; break;
        case '(': t.type = LPAREN; break;
        case ')': t.type = RPAREN; break;
        case '0': case '1':
        case '2': case '3':
        case '4': case '5':
        case '6': case '7':
        case '8': case '9':
            t.type = NUM;
            t.value = (float)atoi(&str[i]);
            while (BETWEEN(str[i], '0', '9')) i++;
            i--;
            // TODO: decimal input
            break;
        case ' ': t.type = SPACE; break;
        default:
            die("tokenize: unknown character `%c`", str[i]);
            break;
        }

        if (t.type != SPACE)
            tokens_append(list, t);
        i++;
    }

    // resize
    list->items = erealloc(list->items, list->count * sizeof(*list->items));
    list->capacity = list->count;

    return list;
}

TreeNode *
parse_addsub(Tokens *list)
{
    TreeNode *node;
    node = parse_muldiv(list);

    const Token *t;
    t = tokens_peek(list);
    if (t && (t->type == ADD || t->type == SUB)) {
        (void)tokens_next(list);
        TreeNode *tmp;
        tmp = node;
        node = emalloc(sizeof(*node));
        node->type = t->type;
        node->binary.left = tmp;
        node->binary.right = parse_addsub(list);
    }
    return node;
}

TreeNode *
parse_muldiv(Tokens *list)
{
    TreeNode *node;
    node = parse_power(list);

    const Token *t;
    t = tokens_peek(list);
    if (t && (t->type == MUL || t->type == DIV)) {
        (void)tokens_next(list);
        TreeNode *tmp;
        tmp = node;
        node = emalloc(sizeof(*node));
        node->type = t->type;
        node->binary.left = tmp;
        node->binary.right = parse_muldiv(list);
    }
    return node;
}

TreeNode *
parse_power(Tokens *list)
{
    TreeNode *node;
    node = parse_parens(list);

    const Token *t;
    t = tokens_peek(list);
    if (t && (t->type == POW)) {
        (void)tokens_next(list);
        TreeNode *tmp;
        tmp = node;
        node = emalloc(sizeof(*node));
        node->type = t->type;
        node->binary.left = tmp;
        node->binary.right = parse_power(list);
    }
    return node;
}

TreeNode *
parse_parens(Tokens *list)
{
    TreeNode *node;

    const Token *t;
    t = tokens_next(list);
    if (!t)
        die("unexpected end of tokens");

    if (t->type == LPAREN) {
        node = parse_addsub(list);
        t = tokens_next(list);
        if (t->type != RPAREN)
            die("mistmatched parens");
    }
    else if (t->type == NUM) {
        node = emalloc(sizeof(*node));
        node->type = NUM;
        node->number = t->value;
    }
    else
        die("parse_parens: invalid type");
    return node;
}

Tokens *
tokens_new(void)
{
    Tokens *list;
    list = emalloc(sizeof(*list));
    list->count = list->index = 0;
    list->capacity = 1;
    list->items = emalloc(sizeof(*list->items));
    return list;
}

void
tokens_destroy(Tokens *list)
{
    free(list->items);
    free(list);
}

void
tokens_append(Tokens *list, Token token)
{
    if (list->count == list->capacity) {
        list->capacity *= 2;
        list->items = erealloc(list->items, list->capacity * sizeof(*list->items));
    }
    list->items[list->count++] = token;
}

const Token *
tokens_peek(Tokens *list)
{
    return (list->index < list->count)
            ? &list->items[list->index]
            : NULL;
}

const Token *
tokens_next(Tokens *list)
{
    return (list->index < list->count)
            ? &list->items[list->index++]
            : NULL;
}

void
tokens_print(Tokens *list)
{
    for (size_t i = 0; i < list->count; i++) {
        Token *t;
        t = &list->items[i];
        switch (t->type) {
        case ADD: printf("+"); break;
        case SUB: printf("-"); break;
        case MUL: printf("*"); break;
        case DIV: printf("/"); break;
        case POW: printf("**"); break;
        case NUM: printf("%.01f", t->value); break;
        case LPAREN: printf("("); break;
        case RPAREN: printf(")"); break;
        default: printf("UNKNOWN(%d)", t->type); break;
        }
        fputc(' ', stdout);
    }
    fputc('\n', stdout);
}

void
tree_destroy(TreeNode *root)
{
    // TODO: this
}

void
tree_print(const TreeNode *root)
{
    tree_print_recurse(root);
    fputc('\n', stdout);
}

void
tree_print_recurse(const TreeNode *root)
{
    switch (root->type) {
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
        fputc('(', stdout);
        tree_print_recurse(root->binary.left);
        if (root->type == ADD) fputc('+', stdout);
        else if (root->type == SUB) fputc('-', stdout);
        else if (root->type == MUL) fputc('*', stdout);
        else if (root->type == DIV) fputc('/', stdout);
        else if (root->type == POW) fprintf(stdout, "**");
        tree_print_recurse(root->binary.right);
        fputc(')', stdout);
        break;
    case NUM:
        printf("%.01f", root->number);
        break;
    default:
        die("tree_print: unexpected type");
        break;
    }
}

