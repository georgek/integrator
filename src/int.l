/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * All rights reserved (at least until the project is finished)
 * 2010 Computer science final year project
 */

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

[a-zA-Z] {
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

extern char *input_line;

void flex_get_rl_input()
{
     /* delete old buffer */
     yy_delete_buffer(YY_CURRENT_BUFFER);
     /* make new buffer which scans string and switch to it */
     yy_scan_string(input_line);
}
