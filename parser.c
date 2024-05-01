#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"
#include "util.h"

typedef enum {
    ADD, SUB, MUL, DIV, POW,
    LPAREN, RPAREN, NUM, SPACE,
} TokenType;

const char *token_type_name[] = {
    "ADD", "SUB", "MUL", "DIV", "POW",
    "LPAREN", "RPAREN", "NUM", "SPACE",
};

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
    Token *list;
    size_t len, index;
    Token const *start;
    Token const *end;
} TokenIter;

static Token *tokenize(const char *str);

static void print_treenode_list(TreeNode *treenode);
static void print_token_list(Token *token);
static size_t listptr2index(void *list, void *ptr, size_t itemsize);

static TreeNode *parse_addsub(TokenIter *iter, TreeNode *list);
static TreeNode *parse_muldiv(TokenIter *iter, TreeNode *list);
static TreeNode *parse_power(TokenIter *iter, TreeNode *list);
static TreeNode *parse_parens(TokenIter *iter, TreeNode *list);

static void tree_print(const TreeNode *root);
static void tree_print_recurse(const TreeNode *root);

static TokenIter *token_iter_create(Token *tokenlist);
static void token_iter_destroy(TokenIter *iter);
static Token *token_iter_next(TokenIter *iter);
static Token *token_iter_peek(TokenIter *iter);

float
mathparser_eval_expr(const char *expr)
{
    Token *token_list;
    token_list = tokenize(expr);
    print_token_list(token_list);

    TokenIter *token_iter;
    TreeNode *node_list, *root;

    token_iter = token_iter_create(token_list);
    da_create(node_list);
    root = parse_addsub(token_iter, node_list);

    print_treenode_list(node_list);

    tree_print(root);

    // da_destroy(list);
    // tree_destroy(tree);
    return 0.0;
}

Token *
tokenize(const char *str)
{
    Token *list;
    size_t len, strpos, i;

    len = strlen(str);
    // list = tokens_new();
    da_create(list);

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
            da_append(list, &t);
            // tokens_append(list, t);
        i++;
    }

    return list;
}

TreeNode *
parse_addsub(TokenIter *iter, TreeNode *list)
{
    TreeNode *node;
    node = parse_muldiv(iter, list);

    const Token *t;
    t = token_iter_peek(iter);
    if (t && (t->type == ADD || t->type == SUB)) {
        (void)token_iter_next(iter);
        TreeNode tmp;
        tmp.type = t->type;
        tmp.binary.left = node;
        tmp.binary.right = parse_addsub(iter, list);
        da_append(list, &tmp);
        node = da_last(list);
    }
    return node;
}

TreeNode *
parse_muldiv(TokenIter *iter, TreeNode *list)
{
    TreeNode *node;
    node = parse_power(iter, list);

    const Token *t;
    t = token_iter_peek(iter);
    if (t && (t->type == ADD || t->type == SUB)) {
        (void)token_iter_next(iter);
        TreeNode tmp;
        tmp.type = t->type;
        tmp.binary.left = node;
        tmp.binary.right = parse_muldiv(iter, list);
        da_append(list, &tmp);
        node = da_last(list);
    }
    return node;
}

TreeNode *
parse_power(TokenIter *iter, TreeNode *list)
{
    TreeNode *node;
    node = parse_parens(iter, list);

    const Token *t;
    t = token_iter_peek(iter);
    if (t && (t->type == ADD || t->type == SUB)) {
        (void)token_iter_next(iter);
        TreeNode tmp;
        tmp.type = t->type;
        tmp.binary.left = node;
        tmp.binary.right = parse_power(iter, list);
        da_append(list, &tmp);
        node = da_last(list);
    }
    return node;
}

TreeNode *
parse_parens(TokenIter *iter, TreeNode *list)
{
    const Token *t;
    t = token_iter_next(iter);
    if (!t)
        die("unexpected end of tokens");

    TreeNode *node;
    if (t->type == LPAREN) {
        node = parse_addsub(iter, list);
        t = token_iter_next(iter);
        if (!t || (t->type != RPAREN))
            die("mismatched parens");
    }
    else if (t->type == NUM) {
        TreeNode tmp;
        tmp.type = NUM;
        tmp.number = t->value;
        da_append(list, &tmp);
        node = da_last(list);
    }
    else
        die("invalid type in parse_parens");
    return node;
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
        die("\ntree_print: unexpected type %d", root->type);
        break;
    }
}

void
print_token_list(Token *token)
{
    for (size_t i = 0; i < da_len(token); i++) {
        switch (token[i].type) {
        case ADD: printf("+"); break;
        case SUB: printf("-"); break;
        case MUL: printf("*"); break;
        case DIV: printf("/"); break;
        case POW: printf("**"); break;
        case NUM: printf("%.01f", token[i].value); break;
        case LPAREN: printf("("); break;
        case RPAREN: printf(")"); break;
        default: printf("UNKNOWN(%d)", token[i].type); break;
        }
    }
    fputc('\n', stdout);
}

size_t
listptr2index(void *list, void *ptr, size_t itemsize)
{
    size_t l, p;
    p = (size_t)list;
    l = (size_t)ptr;
    return ((size_t)(ptr - list)) / itemsize;
}

void
print_treenode_list(TreeNode *treenode)
{
    for (size_t i = 0; i < da_len(treenode); i++) {
        printf("%lu -> { type: %s, ",
               listptr2index(treenode, &treenode[i], sizeof(*treenode)), token_type_name[treenode[i].type]);
        if (treenode[i].type == NUM)
            printf("value: %.02f", treenode[i].number);
        else if (BETWEEN(treenode[i].type, ADD, POW))
            printf("left: %lu, right: %lu", 
                   listptr2index(treenode, treenode[i].binary.left, sizeof(*treenode)),
                   listptr2index(treenode, treenode[i].binary.right, sizeof(*treenode)));
        printf(" }\n");
    }
}

TokenIter *
token_iter_create(Token *tokenlist)
{
    TokenIter *iter;
    iter = emalloc(sizeof(*iter));
    iter->start = iter->list = tokenlist;
    iter->len = da_len(tokenlist);
    iter->end = &iter->list[iter->len-1];
    iter->index = 0;
    return iter;
}

void
token_iter_destroy(TokenIter *iter)
{
    free(iter);
}

Token *
token_iter_next(TokenIter *iter)
{
    return (iter->index < iter->len)
        ? &iter->list[iter->index++]
        : NULL;
}

Token *
token_iter_peek(TokenIter *iter)
{
    return (iter->index < iter->len)
        ? &iter->list[iter->index]
        : NULL;
}

