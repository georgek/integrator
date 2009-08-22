%{
#include <math.h>

#include "tree.h"
#include "y.tab.h"

void yyerror(char *s);
%}

UINT 0|[1-9][0-9]*

%%

{UINT} {
     yylval.ival = atoi(yytext);
     return INTEGER;
}

({UINT}?\.[0-9]+)([eE][+-]?[0-9]+)? {
     yylval.fval = atof(yytext);
     return FLOAT;
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

"e" {
     yylval.fval = exp(1);
     return FLOAT;
}

"pi" {
     yylval.fval = 4*atan(1);
     return FLOAT;
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

