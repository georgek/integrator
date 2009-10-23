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
     BigNum big = make_bignum ("-9847569234923456789123456789123456789123456789", 47);
     BigNum big2 = make_bignum("756789123456789123456789123456789123", 36);
     BigNum big3 = sub_bignums(big, big2);
     print_bignum(big);
     printf("\n");
     print_bignum(big2);
     printf("\n");
     print_bignum(big3);
     printf("\n");
     printf("%d\n", bignum_lte(big, big2));
     free_bignum(big);
     free_bignum(big2);
     free_bignum(big3);
     big = big2 = big3 = NULL;

     yyparse();
     return 0;
}