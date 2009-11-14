%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "bignum.h"
#include "tree.h"
#include "simplify.h"

#define YYERROR_VERBOSE

void yyerror(char *s);

node_type *root = NULL;         /* root of parse tree */
%}

%union {
     int ival;                  /* integer value */
     double fval;               /* floating point value */
     char var;                  /* variable name */
     node_type *node_ptr;
};

%token <ival> INTEGER
%token <fval> FLOAT
%token <var> VARIABLE
%token LN EXP COS SIN TAN EVAL DIFF

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS UPLUS
%left '^'

%type <node_ptr> expression

%%

statement_list: statement '\n'
        |       statement_list statement '\n'
        ;

statement:      expression
                        {
                             root = $1;
                             simple_simplify(&root);
                             print_prefix(root);
                             print_postfix(root);
                             print_prefix_lisp(root);
                             print_infix(root);
                             free_tree(root);
                             root = NULL;
                        }
        |       error
                        {
                             printf("error\n");
                        }
        ;

expression:     expression '+' expression
                        {
                             $$ = add_op2('+', $1, $3);
                        }
        |       expression '-' expression
                        {
                             $$ = add_op2('-', $1, $3);
                        }
        |       expression '*' expression
                        {
                             $$ = add_op2('*', $1, $3);
                        }
        |       expression '/' expression
                        {
                             $$ = add_op2('/', $1, $3);
                        }
        |       '-' expression %prec UMINUS
                        {
                             $$ = add_op1(UMINUS, $2);
                        }
        |       '+' expression %prec UPLUS
                        {
                             $$ = $2;
                        }
        |       expression '^' expression
                        {
                             $$ = add_op2('^', $1, $3);
                        }
        |       LN '(' expression ')'
                        {
                             $$ = add_op1(LN, $3);
                        }
        |       SIN '(' expression ')'
                        {
                             $$ = add_op1(SIN, $3);
                        }
        |       COS '(' expression ')'
                        {
                             $$ = add_op1(COS, $3);
                        }
        |       TAN '(' expression ')'
                        {
                             $$ = add_op1(TAN, $3);
                        }
        |       '(' expression ')'
                        {
                             $$ = $2;
                        }
        |       INTEGER
                        {
                             $$ = add_int($1);
                        }
        |       FLOAT
                        {
                             $$ = add_float($1);
                        }
        |       VARIABLE
                        {
                             $$ = add_var($1);
                        }
        ;

%%

int main (int argc, char *argv[])
{
     int i;
     SHORT_INT_T r;
     BigNum big1 = make_bignuml("83289472398457239847324");
     BigNum big2 = make_bignuml("52293847329847329847");
     BigNum big3 = NULL;
     BigNum big4 = NULL;

     print_bignum(big1);
     printf("\n");

     print_bignum(big2);
     printf("\n");

     div_bignums(&big3, &big4, big1, big2);
     
     print_bignum(big3);
     printf("\n");

     print_bignum(big4);
     printf("\n");

     free_bignum(big1);
     free_bignum(big2);
     free_bignum(big3);
     free_bignum(big4);

     /* yyparse(); */
     return 0;
}
