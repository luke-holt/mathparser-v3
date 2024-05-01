# Overview
This time, I use recursive descent with a context-free grammar.

## Incorrect Implementation
The Backus-Naur grammar that I define below is incorrect. It parses the math expression assuming every operation is right-associative. This parser respects the grammer defined below, but is not useful for parsing and evaluating math expressions. See v4 for the (hopefully) correct implementation.

# Example Usage
```sh
$ ./parsemath
usage: ./parsemath <expr>
supported operations: [ +, -, *, /, ** ]

$ ./parsemath "1 + 1 / 5"
tree node list:
0 -> { type: NUM, value: 1.00 }
1 -> { type: NUM, value: 1.00 }
2 -> { type: NUM, value: 5.00 }
3 -> { type: DIV, left: 1, right: 2 }
4 -> { type: ADD, left: 0, right: 3 }
tree: (1.0+(1.0/5.0))

$ ./parsemath "5 - 4 - 3 - 2 - 1"
tree node list:
0 -> { type: NUM, value: 5.00 }
1 -> { type: NUM, value: 4.00 }
2 -> { type: NUM, value: 3.00 }
3 -> { type: NUM, value: 2.00 }
4 -> { type: NUM, value: 1.00 }
5 -> { type: SUB, left: 3, right: 4 }
6 -> { type: SUB, left: 2, right: 5 }
7 -> { type: SUB, left: 1, right: 6 }
8 -> { type: SUB, left: 0, right: 7 }
tree: (5.0-(4.0-(3.0-(2.0-1.0))))
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
