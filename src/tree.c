/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "bignum.h"
#include "tree.h"
#include "parser.h"
#include "variables.h"

/* size of node without contents */
#define SIZEOF_NODE ((char *)&p->u.coef - (char *)p)

node_type *add_rat(BigNum value)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(coef_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* initialise */
     p->type = coef_type;
     p->u.coef.type = rational;
     p->u.coef.u.rat.num = value;
     p->u.coef.u.rat.den = make_bignum2(1);

     return p;
}

node_type *add_var(char name)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(coef_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* initialise */
     p->type = coef_type;
     p->u.coef.type = polynomial;
     p->u.coef.u.poly = make_mono_poly(name, 1);

     return p;
}

node_type *add_poly(Polynomial poly)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(coef_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* initialise */
     p-> type = coef_type;
     p->u.coef.type = polynomial;
     p->u.coef.u.poly = poly;

     return p;
}

node_type *add_ratfun(Coefficient num, Coefficient den)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(ratfun_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* copy information */
     p->type = ratfun_type;
     p->u.ratfun.num = num;
     p->u.ratfun.den = den;
     canonicalise_ratfun(&p->u.ratfun);

     return p;
}

node_type *add_op1(int operator, node_type *operand)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(op1_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* copy information */
     p->type = op1_type;
     p->u.op1.operator = operator;
     p->u.op1.operand = operand;

     return p;
}

node_type *add_op2(int operator, node_type *operand1, node_type *operand2)
{
     node_type *p;
     size_t node_size;

     node_size = SIZEOF_NODE + sizeof(op2_node_type);
     if ((p = malloc(node_size)) == NULL) {
          printf("out of memory\n");
     }

     /* copy information */
     p->type = op2_type;
     p->u.op2.operator = operator;
     p->u.op2.operand1 = operand1;
     p->u.op2.operand2 = operand2;

     return p;
}

void free_tree(node_type *p)
{
     if (!p) {
          return;
     }
     else if (p->type == coef_type) {
          free_coefficient(&p->u.coef);
     }
     else if (p->type == op1_type) {
          free_tree(p->u.op1.operand);
     }
     else if (p->type == op2_type) {
          free_tree(p->u.op2.operand1);
          free_tree(p->u.op2.operand2);
     }
     free(p);
}

void print_prefix_lisp(node_type *root)
{
     traverse_prefix_lisp(root, 0);
     printf("\n");
}

void traverse_prefix_lisp(node_type *p, int prev_op)
{
     if (!p) return;
     switch (p->type) {
     case coef_type:
          print_coefficient(p->u.coef);
          break;
     case ratfun_type:
          /* print_ratfun(p->u.ratfun); */
          /* printf("\n"); */
          print_ratfun(p->u.ratfun);
          break;
     case op1_type:
          printf("(");
          print_operator(p->u.op1.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op1.operand, p->u.op1.operator);
          printf(")");
          break;
     case op2_type:
          /* if last operator was the same as this one, don't print it again */
          if (p->u.op2.operator == prev_op && prev_op != '^') {
               traverse_prefix_lisp(p->u.op2.operand1,
                                    p->u.op2.operator);
               printf(" ");
               traverse_prefix_lisp(p->u.op2.operand2,
                                    p->u.op2.operator);
               break;
          }
          printf("(");
          print_operator(p->u.op2.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op2.operand1, p->u.op2.operator);
          printf(" ");
          traverse_prefix_lisp(p->u.op2.operand2, p->u.op2.operator);
          printf(")");
          break;
     }
}

void print_prefix(node_type *root)
{
     traverse_prefix(root);
     printf("\n");
}

void traverse_prefix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case coef_type:
          print_coefficient(p->u.coef);
          break;
     case ratfun_type:
          print_ratfun(p->u.ratfun);
          break;
     case op1_type:
          print_operator(p->u.op1.operator);
          printf(" ");
          traverse_prefix(p->u.op1.operand);
          break;
     case op2_type:
          print_operator(p->u.op2.operator);
          printf(" ");
          traverse_prefix(p->u.op2.operand1);
          printf(" ");
          traverse_prefix(p->u.op2.operand2);
          break;
     }
}

void print_postfix(node_type *root)
{
     traverse_postfix(root);
     printf("\n");
}

void traverse_postfix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case coef_type:
          print_coefficient(p->u.coef);
          break;
     case ratfun_type:
          print_ratfun(p->u.ratfun);
          break;
     case op1_type:
          traverse_postfix(p->u.op1.operand);
          printf(" ");
          print_operator(p->u.op1.operator);
          break;
     case op2_type:
          traverse_postfix(p->u.op2.operand1);
          printf(" ");
          traverse_postfix(p->u.op2.operand2);
          printf(" ");
          print_operator(p->u.op2.operator);
          break;
     }
}

void print_infix(node_type *root)
{
     traverse_infix(root);
     printf("\n");
}

void traverse_infix(node_type *p)
{
     if (!p) return;
     switch (p->type) {
     case coef_type:
          print_coefficient(p->u.coef);
          break;
     case ratfun_type:
          print_ratfun(p->u.ratfun);
          break;
     case op1_type:
          /* deal with unary minus */
          if (p->u.op1.operator == UMINUS) {
               printf("-");
               if ((p->u.op1.operand->type == op2_type)
                   && (higher_priority(p->u.op1.operator,
                                       p->u.op1.operand->u.op2.operator)
                       ==1)) {
                    printf("(");
                    traverse_infix(p->u.op1.operand);
                    printf(")");
               }
               else {
                    traverse_infix(p->u.op1.operand);
               }
          }
          else {
               print_operator(p->u.op1.operator);
               printf("(");
               traverse_infix(p->u.op1.operand);
               printf(")");
          }
          break;
     case op2_type:
          /* if one of the operands is an operator of lower priority
           * than this operator, it must go in brackets */
          if ((p->u.op2.operand1->type == op2_type)
              && (higher_priority(p->u.op2.operator,
                                  p->u.op2.operand1->u.op2.operator)
                  == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand1);
               printf(")");
          }
          else if ((p->u.op2.operand1->type == op1_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand1->u.op1.operator)
                       == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand1);
               printf(")");
          }
          else {
               traverse_infix(p->u.op2.operand1);
          }

          /* printf(" "); */
          print_operator(p->u.op2.operator);
          /* printf(" "); */

          if ((p->u.op2.operand2->type == op2_type)
              && (higher_priority(p->u.op2.operator,
                                  p->u.op2.operand2->u.op2.operator)
                  == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          /* same priority but different operator, must have had brackets */
          else if ((p->u.op2.operand2->type == op2_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand2->u.op2.operator)
                       == 0)
                   && p->u.op2.operator != p->u.op2.operand2->u.op2.operator) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          else if ((p->u.op2.operand2->type == op1_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand2->u.op1.operator)
                       == 1)) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          else if ((p->u.op2.operand2->type == op1_type)
                   && (higher_priority(p->u.op2.operator,
                                       p->u.op2.operand2->u.op1.operator)
                       == 0)
                   && p->u.op2.operator != p->u.op2.operand2->u.op1.operator) {
               printf("(");
               traverse_infix(p->u.op2.operand2);
               printf(")");
          }
          else {
               traverse_infix(p->u.op2.operand2);
          }

          break;
     }
}

/* returns operator's precedence for infix mode */
int op_priority(int i)
{
     int p;
     switch (i)
     {
     case '-': p=1; break;
     case '+': p=1; break;
     case '*': p=2; break;
     case '/': p=2; break;
     case UMINUS: p=4; break;
     case '^': p=5; break;
     default: p=6;break;
     }
     return (p);
}

/* compares operator priority for infix mode */
int higher_priority(int i, int j)
{
     int p1, p2;
     p1=op_priority(i); p2=op_priority(j);
     if (p1>p2) return 1 ;
     else if (p1<p2) return -1 ;
     else return 0;
}

void print_operator(int id)
{
     switch (id) {
     case LN:
          printf("ln");
          break;
     case EXP:
          printf("exp");
          break;
     case SIN:
          printf("sin");
          break;
     case COS:
          printf("cos");
          break;
     case TAN:
          printf("tan");
          break;
     case UMINUS:
          printf("~");
          break;
     case UPLUS:
          printf("P");
          break;
     default:
          printf("%c", id);
          break;
     }
}

void extract_polys(node_type **root)
{
     node_type *r = *root;
     Coefficient tq = {special}, tr = {special};

     switch (r->type) {
     case op2_type:
          extract_polys(&(r->u.op2.operand1));
          extract_polys(&(r->u.op2.operand2));

          if (r->u.op2.operand1->type != coef_type
              || r->u.op2.operand2->type != coef_type) {
               /* nothing to do here */
               return;
          }

          switch (r->u.op2.operator) {
          case '^':
               if (r->u.op2.operand2->u.coef.type != rational
                   || real_length(r->u.op2.operand2->u.coef.u.rat.num) != 1) {
                    printf("Error! "
                           "Indices must be single precision integers!\n");
                    exit(1);
               }
               if (coef_neg(r->u.op2.operand2->u.coef)) {
                    /* this is a rational function */
                    break;
               }

               coef_power(&r->u.op2.operand1->u.coef,
                          r->u.op2.operand1->u.coef,
                          *(r->u.op2.operand2->u.coef.u.rat.num+1));
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '+':
               add_coefficients(&r->u.op2.operand1->u.coef,
                                r->u.op2.operand1->u.coef,
                                r->u.op2.operand2->u.coef);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '-':
               sub_coefficients(&r->u.op2.operand1->u.coef,
                                r->u.op2.operand1->u.coef,
                                r->u.op2.operand2->u.coef);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '*':
               mul_coefficients(&r->u.op2.operand1->u.coef,
                                r->u.op2.operand1->u.coef,
                                r->u.op2.operand2->u.coef);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '/':
               /* see if division will be exact */
               polydiv_coefficients(&tq, &tr,
                                    r->u.op2.operand1->u.coef,
                                    r->u.op2.operand2->u.coef);
               if (!coef_zero(tr)) {
                    /* division is not exact so this is a ratfun */
                    /* PRINTC(tq); */
                    /* PRINTC(tr); */
                    free_coefficient(&tq);
                    free_coefficient(&tr);

                    *root = add_ratfun(r->u.op2.operand1->u.coef,
                                       r->u.op2.operand2->u.coef);
                    r->u.op2.operand1->u.coef.type = special;
                    r->u.op2.operand2->u.coef.type = special;
                    free_tree(r);
                    break;
               }

               /* division is exact */
               copy_coefficient(&r->u.op2.operand1->u.coef, tq);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               free_coefficient(&tq);
               free_coefficient(&tr);
               break;
          }
          break;

     case op1_type:
          extract_polys(&r->u.op1.operand);

          if (r->u.op1.operand->type != coef_type) {
               /* nothing to do here */
               return;
          }

          switch (r->u.op1.operator) {
          case UMINUS:
               negate_coefficient(&r->u.op1.operand->u.coef);
               *root = r->u.op1.operand;
               r->u.op1.operand = NULL;
               free_tree(r);
               break;
          }
          break;

     case coef_type:
     case ratfun_type:
          break;
     }
}

void extract_ratfuns(node_type **root)
{
     node_type *r = *root;
     Coefficient t = {special};

     switch (r->type) {
     case op2_type:
          extract_ratfuns(&(r->u.op2.operand1));
          extract_ratfuns(&(r->u.op2.operand2));

          if (r->u.op2.operand1->type != ratfun_type
              || r->u.op2.operand2->type != ratfun_type) {
               /* nothing to do here */
               return;
          }


          switch (r->u.op2.operator) {
          case '^':
               if (r->u.op2.operand2->u.coef.type != rational
                   || real_length(r->u.op2.operand2->u.coef.u.rat.num) != 1) {
                    printf("Error! "
                           "Indices must be single precision integers!\n");
                    exit(1);
               }

               ratfun_power(&r->u.op2.operand1->u.ratfun,
                            r->u.op2.operand1->u.ratfun,
                            *(r->u.op2.operand2->u.coef.u.rat.num+1));
               if (coef_neg(r->u.op2.operand2->u.coef)) {
                    invert_ratfun(&r->u.op2.operand1->u.ratfun);
               }
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '+':
               add_ratfuns(&r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand2->u.ratfun);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '-':
               sub_ratfuns(&r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand2->u.ratfun);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '*':
               mul_ratfuns(&r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand2->u.ratfun);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;

          case '/':
               div_ratfuns(&r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand1->u.ratfun,
                           r->u.op2.operand2->u.ratfun);
               *root = r->u.op2.operand1;
               r->u.op2.operand1 = NULL;
               free_tree(r);
               break;
          }
          break;

     case op1_type:
          extract_ratfuns(&r->u.op1.operand);

          if (r->u.op1.operand->type != ratfun_type) {
               /* nothing to do here */
               return;
          }

          switch (r->u.op1.operator) {
          case UMINUS:
               negate_ratfun(&r->u.op1.operand->u.ratfun);
               *root = r->u.op1.operand;
               r->u.op1.operand = NULL;
               free_tree(r);
               break;
          }
          break;

     case coef_type:
          /* turn into ratfun */
          t.type = rational;
          t.u.rat = make_bigrat3(1);
          *root = add_ratfun(r->u.coef, t);
          r->u.coef.type = special;
          free_tree(r);
     case ratfun_type:
          break;
     }
}

extern char main_var;

void set_main_var(node_type **root)
{
     node_type *r = *root;

     if (r->type != op2_type || r->u.op2.operator != ',') {
          /* x by default */
          set_top_var('x');
          main_var = 'x';
          return;
     }

     if (r->u.op2.operand2->type == coef_type) {
          set_top_var(r->u.op2.operand2->u.coef.u.poly.variable);
          main_var = r->u.op2.operand2->u.coef.u.poly.variable;
          *root = r->u.op2.operand1;
          r->u.op2.operand1 = NULL;
          free_tree(r);
          return;
     }
     else if (r->u.op2.operand2->type == op2_type) {
          set_main_var(&r->u.op2.operand2);
     }
}
