%left PLUS.
%left TIMES.
s ::= expr.
expr ::= expr PLUS expr.
expr ::= expr TIMES expr.
expr ::= LPAREN expr RPAREN.
expr ::= VALUE.
