/* parser for integrator front end */

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "bignum.h"
#include "bigrat.h"
#include "tree.h"
#include "simplify.h"
#include "polynomial.h"
#include "euclidean.h"
#include "coef_array.h"
#include "prs.h"
#include "squarefree.h"

#define YYERROR_VERBOSE

void yyerror(char *s);
int yylex(void);

node_type *root = NULL;         /* root of parse tree */
%}

%union {
     BigNum ival;               /* integer value */
     char var;                  /* variable name */
     node_type *node_ptr;
};

%token <ival> INTEGER
%token <var> VARIABLE
%token LN EXP COS SIN TAN EVAL DIFF

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
                             simple_simplify(&root);
                             extract_polys(&root);
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
                             $$ = add_rat($1);
                        }
        |       VARIABLE
                        {
                             $$ = add_var($1);
                        }
        ;

%%

int main (int argc, char *argv[])
{
     /* int i; */
     /* unsigned t1, t2; */
     /* BigNum big1 = NULL, big2 = NULL, big3 = NULL, big4 = NULL, big5 = NULL; */
     /* BigRat rat1, rat2, rat3; */

     /* if (argc < 5) { */
     /*      printf("not enough args, give four integers\n"); */
     /*      return 1; */
     /* } */

     /* t1 = 1; */

     /* big1 = make_bignuml(argv[1]); */
     /* big2 = make_bignuml(argv[2]); */
     /* big3 = make_bignuml(argv[3]); */
     /* big4 = make_bignuml(argv[4]); */

     /* init_bigrat(&rat1); */
     /* init_bigrat(&rat2); */
     /* init_bigrat(&rat3); */

     /* make_bigrat(&rat1, big1, big2); */
     /* make_bigrat(&rat2, big3, big4); */

     /* for (i = 0; i < 10; ++i) { */
     /*      mul_bigrats(&rat1, rat1, rat1); */
     /* } */

     /* div_bigrats(&rat2, rat2, rat2); */
     
     /* sub_bigrats(&rat3, rat1, rat2); */
     /* add_bigrats(&rat1, rat2, rat3); */

     /* print_bigrat(rat1); */
     /* printf("\n"); */
     /* print_bigrat(rat2); */
     /* printf("\n"); */
     /* print_bigrat(rat3); */
     /* printf("\n"); */

     /* free_bignum(big1); */
     /* free_bignum(big2); */
     /* free_bignum(big3); */
     /* free_bignum(big4); */
     /* free_bignum(big5); */

     /* free_bigrat(&rat1); */
     /* free_bigrat(&rat2); */
     /* free_bigrat(&rat3); */

     /* test some inline assembler */
     /* asm volatile ("ror %0" : "=r" (t2) : "0" (t1)); */

     /* printf("%u\n%u\n", t1, t2); */

     /* Polynomial poly1 = make_zero_poly('x'), poly2 = make_zero_poly('x'), */
     /*      poly3 = make_zero_poly('x'), poly4 = make_zero_poly('x'), */
     /*      poly5 = make_zero_poly('x'); */
     /* BigNum big1 = NULL, big2 = NULL, big3 = NULL, big4 = NULL, big5 = NULL, */
     /*      big6 = NULL, big7 = NULL; */
     /* Coefficient coef1, coef2, coef3, coef4, coef5, coef6, coef7; */
     /* CoefArray arr = new_coef_array(); */
     /* CoefPtr t; */
     /* unsigned i; */

     /* big1 = make_bignum2(1); */
     /* big2 = make_bignum2(6); */
     /* /\* negate_bignum(big2); *\/ */
     /* big3 = make_bignum2(12); */
     /* /\* negate_bignum(big3); *\/ */
     /* big4 = make_bignum2(8); */
     /* big5 = make_bignum2(15); */
     /* big6 = make_bignum2(4); */
     /* negate_bignum(big6); */
     /* big7 = make_bignum2(1); */
     /* negate_bignum(big7); */

     /* coef1.type = rational; */
     /* init_bigrat(&coef1.u.rat); */
     /* bignum_copy(&coef1.u.rat.num, big1); */
     /* bignum_copy(&coef1.u.rat.den, big1); */

     /* coef2.type = rational; */
     /* init_bigrat(&coef2.u.rat); */
     /* bignum_copy(&coef2.u.rat.num, big2); */
     /* bignum_copy(&coef2.u.rat.den, big1); */

     /* coef3.type = rational; */
     /* init_bigrat(&coef3.u.rat); */
     /* bignum_copy(&coef3.u.rat.num, big3); */
     /* bignum_copy(&coef3.u.rat.den, big1); */

     /* coef4.type = rational; */
     /* init_bigrat(&coef4.u.rat); */
     /* bignum_copy(&coef4.u.rat.num, big4); */
     /* bignum_copy(&coef4.u.rat.den, big1); */

     /* coef5.type = rational; */
     /* init_bigrat(&coef5.u.rat); */
     /* bignum_copy(&coef5.u.rat.num, big5); */
     /* bignum_copy(&coef5.u.rat.den, big1); */

     /* coef6.type = rational; */
     /* init_bigrat(&coef6.u.rat); */
     /* bignum_copy(&coef6.u.rat.num, big6); */
     /* bignum_copy(&coef6.u.rat.den, big1); */

     /* coef7.type = rational; */
     /* init_bigrat(&coef7.u.rat); */
     /* bignum_copy(&coef7.u.rat.num, big7); */
     /* bignum_copy(&coef7.u.rat.den, big1); */

     /* free_bignum(big1); */
     /* free_bignum(big2); */
     /* free_bignum(big3); */
     /* free_bignum(big4); */
     /* free_bignum(big5); */
     /* free_bignum(big6); */
     /* free_bignum(big7); */

     /* print_poly(poly1); */
     /* printf("\n"); */

     /* add_monomial(&poly1, 8, coef1); */
     /* add_monomial(&poly1, 6, coef2); */
     /* add_monomial(&poly1, 4, coef3); */
     /* add_monomial(&poly1, 2, coef4); */
     /* /\* add_monomial(&poly1, 0, coef5); *\/ */
     
     /* add_monomial(&poly2, 3, coef1); */
     /* add_monomial(&poly2, 2, coef1); */
     /* add_monomial(&poly2, 1, coef6); */
     /* add_monomial(&poly2, 0, coef6); */

     /* /\* add_monomial(&poly2, 2, coef1); *\/ */
     /* /\* add_monomial(&poly2, 0, coef7); *\/ */

     /* free_bigrat(&coef1.u.rat); */
     /* free_bigrat(&coef2.u.rat); */
     /* free_bigrat(&coef3.u.rat); */
     /* free_bigrat(&coef4.u.rat); */
     /* free_bigrat(&coef5.u.rat); */
     /* free_bigrat(&coef6.u.rat); */
     /* free_bigrat(&coef7.u.rat); */

     /* /\* SolveDiophantineEquation(&poly4, &poly5, poly1, poly2, poly3); *\/ */
     /* /\* exact_div_polynomials(&poly4, poly1, poly2); *\/ */
     /* poly_differentiate(&poly2, poly1); */
     /* /\* Euclidean(&poly4, poly1, poly2); *\/ */
     /* /\* SubResultant(&poly4, &arr, poly1, poly2); *\/ */
     /* /\* SubResultantGCD(&poly3, poly1, poly2); *\/ */
     /* Squarefree(&arr, poly1); */

     /* print_poly(poly1); */
     /* printf("\n"); */
     /* print_poly(poly2); */
     /* printf("\n"); */
     /* print_poly(poly3); */
     /* printf("\n"); */
     /* print_poly(poly4); */
     /* printf("\n"); */
     /* print_poly(poly5); */
     /* printf("\n"); */

     /* /\* for (; i < 198; ++i) { *\/ */
     /* /\*      t = ca_poly_push_back(&arr, poly1); *\/ */
     /* /\*      t->u.poly.head->next->next->coeff.u.rat.num = make_bignum2(i+1); *\/ */
     /* /\* } *\/ */
     /* for (i = 0; i < arr.size; ++i) { */
     /*      print_coefficient(ca_get(&arr, i)); */
     /*      printf(" <-- %u\n", i); */
     /* } */
     /* printf("%u, %u\n", arr.size, arr.max_size); */

     /* free_poly(&poly1); */
     /* free_poly(&poly2); */
     /* free_poly(&poly3); */
     /* free_poly(&poly4); */
     /* free_poly(&poly5); */

     yyparse();
     return 0;
}
