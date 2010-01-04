%{
#include <math.h>

#include "bignum.h"
#include "tree.h"
#include "y.tab.h"

void yyerror(char *s);
%}

UINT 0|[1-9][0-9]*

%%

{UINT} {
     yylval.ival = make_bignum(yytext, yyleng);
     return INTEGER;
}

[xyz] {
     yylval.var = *yytext;
     return VARIABLE;
}

"ln" {
     return LN;
}

"sin" {
     return SIN;
}

"cos" {
     return COS;
}

"tan" {
     return TAN;
}

"diff" {
     return DIFF;
}

"eval" {
     return EVAL;
}

[-+*/^(),\n] {
     return *yytext;
}

[ \t]+ {
     ;                          /* ignore whitespace */
}

"$" {
     return 0;                  /* end of input */
}

. {
     fprintf(stdout,"Character '%c' not allowed in expression.\n",*yytext);
     yyerror("Unknown character found by lexical analyser.");
}

%%

