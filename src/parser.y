/* parser for integrator front end */

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "bignum.h"
#include "bigrat.h"
#include "tree.h"
#include "polynomial.h"
#include "euclidean.h"
#include "coef_array.h"
#include "prs.h"
#include "squarefree.h"
#include "hermite.h"
#include "variables.h"

int yylex(void);

void yyerror(char *s)
{
     fprintf(stderr, "%s\n", s);
}

node_type *root = NULL;         /* root of parse tree */
%}

%union {
     BigNum ival;               /* integer value */
     char var;                  /* variable name */
     node_type *node_ptr;
};

%token <ival> INTEGER
%token <var> VARIABLE
%token LN EXP COS SIN TAN EVAL DIFF QUIT

%left ','
%left '+' '-'
%left '*'
%left '/'
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
                             /* simple_simplify(&root); */
                             set_main_var(&root);
                             printf("Variable table:\n");
                             print_var_tab();
                             printf("Input:\n");
                             print_prefix(root);
                             print_postfix(root);
                             print_prefix_lisp(root);
                             print_infix(root);
                             extract_polys(&root);
                             /* extract_ratfuns(&root); */
                             printf("---\n");
                             print_prefix_lisp(root);
                             printf("Integral:\n");
                             HermiteReduceI(root);
                             /* printf("Squarefree factorisation:\n"); */
                             /* SquarefreeI(root); */
                             /* printf("GCD:\n"); */
                             /* GCDI(root); */
                             free_tree(root);
                             root = NULL;
                             make_var_tab();
                             printf("-> ");
                        }
        |       error
                        {
                             printf("-> ");
                        }
        |       QUIT
                        {
                             exit(0);
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
        |       expression ',' expression
                        {
                             $$ = add_op2(',', $1, $3);
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
                             $$ = add_rat($1);
                        }
        |       VARIABLE
                        {
                             $$ = add_var($1);
                             add_var_cell($1);
                        }
        ;

%%

int main (int argc, char *argv[])
{
     printf("Integrator.\n"
            "Enter some expressions in x:\n");

     printf("-> ");
     yyparse();
     return 0;
}
