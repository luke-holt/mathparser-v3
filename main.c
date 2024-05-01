#include <stdio.h>

#include "parser.h"
#include "util.h"

int
main(int argc, char *argv[])
{
    if (argc != 2)
        die("usage: %s <expr>\nsupported operations: [ +, -, *, /, ** ]", argv[0]);

    mathparser_eval_expr(argv[1]);

    return 0;
}
