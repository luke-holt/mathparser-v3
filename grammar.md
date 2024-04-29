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
