#include <stdlib.h>

typedef enum {
    ADD, SUB, MUL, DIV, POW,
    LPAREN, RPAREN, NUM,
} TokenType;

typedef enum {
    ADDSUB, MULDIV, POWER, PARENS,
    NUMBER, DECIMAL, INTEGER, DIGIT,
} OpType;

typedef struct TreeNode TreeNode;
struct TreeNode {
    OpType type;
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
    Token *arr;
    size_t count, capacity, index;
} Tokens;

static TreeNode *parse_addsub(Tokens *list);
static TreeNode *parse_muldiv(Tokens *list);
static TreeNode *parse_power(Tokens *list);
static TreeNode *parse_parens(Tokens *list);
static TreeNode *parse_number(Tokens *list);

TreeNode *
parse_addsub(Tokens *list)
{
}

TreeNode *
parse_muldiv(Tokens *list)
{
}

TreeNode *
parse_power(Tokens *list)
{
}

TreeNode *
parse_parens(Tokens *list)
{
}

TreeNode *
parse_number(Tokens *list)
{
}

