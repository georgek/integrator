/* parser for integrator front end */

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include <readline/readline.h>
#include <readline/history.h>

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

char *rl_gets(int lineno);
void initialise_readline();
void flex_get_rl_input();

node_type *root = NULL;         /* root of parse tree */

char* input_line = NULL;        /* line of input from readline */
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

/* statement_list: statement '\n' */
/*         |       statement_list statement '\n' */
/*         ; */

statement:      expression '\n'
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
                             return 0;
                        }
        |       error
                        {
                             return 2;
                        }
        |       QUIT
                        {
                             /* exit(0); */
                             return 1;
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
     int parseret = 0;
     unsigned lineno = 1;

     initialise_readline();

     printf("Integrator.\n"
            "Enter expressions in x (by default).\n"
            "Type quit, or enter an EOF to quit.\n");

     /* user input prompt */
     while (1) {
          input_line = rl_gets(lineno);
          if (input_line == NULL) {
               printf("\n");
               return 0;
          }
          input_line = strcat(input_line, "\n"); /* put nl back on */
          flex_get_rl_input();
          parseret = yyparse();
          if (parseret == 0) {
               /* normal input */
               ++lineno;
          }
          else if (parseret == 1) {
               /* exit */
               break;
          }
          else if (parseret == 2) {
               /* error, delete rest of line */
               free(input_line);
               input_line = NULL;
          }
     }
     free(input_line);
     input_line = NULL;

     return 0;
}

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *rl_gets (int lineno)
{
     char prompt[12];
     
     /* If the buffer has already been allocated, return the memory to the
        free pool. */
     if (input_line) {
          free(input_line);
          input_line = NULL;
     }

     /* Get a line from the user. */
     sprintf(prompt, "(%d) -> ", lineno);
     input_line = readline(prompt);

     /* If the line has any text in it, save it on the history. */
     if (input_line && *input_line) {
          add_history(input_line);
     }

     return input_line;
}

void initialise_readline()
{
     /* make tab just insert a tab */
     rl_bind_key ('\t', rl_insert);
}
