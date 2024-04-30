# Overview
Convert a math expression to it's equivalent in Reverse Polish Notation (RPN).
This time, I use recursive descent with a context-free grammar.

# Example Usage
```
$ ./parsemath
usage: ./parsemath <expr>
supported operations: [ +, -, *, /, ** ]
$ ./parsemath "1 + 1"
tokens: 1 + 1
rpn: 1 1 +
result: 2
$ ./parsemath "(24 - 9 * 2) ** 2"
tokens: ( 24 - 9 * 2 ) ** 2
rpn: 24 9 2 * - 2 **
result: 36
$ ./parsemath "108 / (2 ** 3 + 4)"
tokens: 108 / ( 2 ** 3 + 4 )
rpn: 108 2 3 ** 4 + /
result: 9
```

# Backus-Naur Grammar
```BNF
ADDSUB   ::= <MULDIV> ("+" | "-") <ADDSUB> | <MULDIV>;
MULDIV   ::= <POWER> ("*" | "/") <MULDIV> | <POWER>;
POWER    ::= <PARENS> "**" <POWER> | <PARENS>;
PARENS   ::= "(" <ADDSUB> ")" | <NUMBER>;
NUMBER   ::= <DECIMAL> | <INTEGER>;
DECIMAL  ::= <INTEGER> "." <INTEGER>;
INTEGER  ::= <DIGIT> <INTEGER> | <DIGIT>;
DIGIT    ::= "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9";
```
