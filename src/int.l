/* lexer for integrator front end */

%option nounput
%option noinput
%option noyywrap
%option always-interactive

%{
#include <math.h>

#include "bignum.h"
#include "tree.h"
#include "parser.h"

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

"exit"|"quit" {
     return QUIT;
}

<<EOF>> {
     printf("\n");
     return QUIT;
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
     printf("Character '%c' not allowed in expression.\n",*yytext);
}

%%

