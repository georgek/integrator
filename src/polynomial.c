/* This file is part of Integrator
 * (C) 2010 George Kettleborough
 * This is free software.  See COPYING file for license.
 * 2010 Computer science final year project
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "polynomial.h"
#include "bigrat.h"
#include "prs.h"
#include "variables.h"

static void rat_coef_op(Coefficient *res, Coefficient left, Coefficient right,
                        void (*op_fun)(BigRat*, BigRat, BigRat));

Polynomial make_zero_poly(char variable)
{
     Polynomial t;
     t.variable = variable;
     t.head = malloc(sizeof(Monomial));
     t.head->degree = 0;
     t.head->coeff.type = special;
     t.head->next = t.head;

     return t;
}

Polynomial make_one_poly(char variable)
{
     Polynomial t = make_zero_poly(variable);
     t.head->next = malloc(sizeof(Monomial));
     t.head->next->next = t.head;
     t.head->next->degree = 0;
     t.head->next->coeff.type = rational;
     t.head->next->coeff.u.rat.num = make_bignum2(1);
     t.head->next->coeff.u.rat.den = make_bignum2(1);
     return t;
}

Polynomial make_const_poly(char variable, BigRat r)
{
     Polynomial t = make_zero_poly(variable);
     t.head->next = malloc(sizeof(Monomial));
     t.head->next->next = t.head;
     t.head->next->degree = 0;
     t.head->next->coeff.type = rational;
     bigrat_copy(&t.head->next->coeff.u.rat, r);
     return t;
}

Polynomial make_mono_poly(char variable, int degree)
{
     Polynomial t = make_one_poly(variable);
     t.head->next->degree = degree;
     return t;
}

void free_poly(Polynomial *p)
{
     MonoPtr q, r;
     if (p->head == NULL) {
          /* empty poly */
          return;
     }
     /* turn mono list into a normal list */
     q = p->head->next;
     p->head->next = NULL;
     for ( ; q != NULL; q=r) {
          r = q->next;
          free_coefficient(&q->coeff);
          free(q);
     }
     p->head = NULL;
}

void copy_poly(Polynomial *p, Polynomial s)
{
     Polynomial old_res = *p;
     MonoPtr q, pq;
     
     p->variable = s.variable;
     p->head = malloc(sizeof(Monomial));
     p->head->degree = 0;
     p->head->coeff.type = special;
     pq = p->head;

     for (q = s.head->next; q->coeff.type != special; q = q->next) {
          pq->next = malloc(sizeof(Monomial));
          pq = pq->next;
          pq->degree = q->degree;
          pq->coeff.type = special;
          copy_coefficient(&pq->coeff, q->coeff);
     }
     pq->next = p->head;

     free_poly(&old_res);
}

/* void print_poly_simple(Polynomial p) */
/* { */
/*      MonoPtr m = p.head->next; */
/*      /\* check for zero polynomial *\/ */
/*      if (m->coeff.type == special) { */
/*           printf("zero"); */
/*           return; */
/*      } */
/*      /\* iterate through monomials *\/ */
/*      do { */
/*           print_coefficient_simple(m->coeff); */
/*           printf("*"); */
/*           printf("%c^%d", p.variable, m->degree); */
/*           m = m->next; */
/*      } while (m->coeff.type != special && printf(" + ")); */
/* } */

/* void print_poly_nonpretty(Polynomial p) */
/* { */
/*      MonoPtr m = p.head->next; */
/*      /\* check for zero polynomial *\/ */
/*      if (m->coeff.type == special) { */
/*           printf("zero"); */
/*           return; */
/*      } */
/*      /\* iterate through monomials *\/ */
/*      do { */
/*           /\* don't print unit coefficient, unless we won't print variable *\/ */
/*           if (!coef_one(m->coeff) || m->degree == 0) { */
/*                print_coefficient_nonpretty(m->coeff); */
/*           } */
/*           /\* coefficient and variable printed? *\/ */
/*           if (!coef_one(m->coeff) && m->degree != 0) { */
/*                printf("*"); */
/*           } */
/*           /\* print variable if degree > 0 *\/ */
/*           if (m->degree > 1) { */
/*                printf("%c^%d", p.variable, m->degree); */
/*           } */
/*           else if (m->degree == 1) { */
/*                printf("%c", p.variable); */
/*           } */
/*           m = m->next; */
/*      } while (m->coeff.type != special && printf(" + ")); */
/* } */

void print_poly(Polynomial p)
{
     MonoPtr m = p.head->next;
     int flipt = 0;
     /* check for zero polynomial */
     if (m->coeff.type == special) {
          printf("zero");
          return;
     }
     /* if (poly_neg(p)) { */
     /*      flip = !flip; */
     /* } */

     /* print first monomial */
     /* sign */
     if (coef_neg(m->coeff)) {
          flipt = 1;
          printf("-");
     }
     /* don't print unit coefficient, unless we won't print variable */
     if (!coef_one2(m->coeff) || m->degree == 0) {
          print_coefficient2(m->coeff, flipt);
     }
     /* coefficient and variable printed? */
     if (!coef_one2(m->coeff) && m->degree != 0) {
          printf("*");
     }
     /* print variable if degree > 0 */
     if (m->degree > 1) {
          printf("%c^%d", p.variable, m->degree);
     }
     else if (m->degree == 1) {
          printf("%c", p.variable);
     }
     m = m->next;

     /* iterate through other monomials */
     while (m->coeff.type != special) {
          /* sign */
          if (coef_neg(m->coeff)) {
               flipt = 1;
               printf(" - ");
          }
          else {
               flipt = 0;
               printf(" + ");
          }
          
          /* don't print unit coefficient, unless we won't print variable */
          if (!coef_one2(m->coeff) || m->degree == 0) {
               print_coefficient2(m->coeff, flipt);
          }
          /* coefficient and variable printed? */
          if (!coef_one2(m->coeff) && m->degree != 0) {
               printf("*");
          }
          /* print variable if degree > 0 */
          if (m->degree > 1) {
               printf("%c^%d", p.variable, m->degree);
          }
          else if (m->degree == 1) {
               printf("%c", p.variable);
          }
          m = m->next;
     }
}

void print_poly2(Polynomial p, int flip)
{
     MonoPtr m = p.head->next;
     int flipt = flip;
     /* check for zero polynomial */
     if (m->coeff.type == special) {
          printf("zero");
          return;
     }

     /* print first monomial */
     /* don't print unit coefficient, unless we won't print variable */
     if (!coef_one2(m->coeff) || m->degree == 0) {
          print_coefficient2(m->coeff, flip);
     }
     /* coefficient and variable printed? */
     if (!coef_one2(m->coeff) && m->degree != 0) {
          printf("*");
     }
     /* print variable if degree > 0 */
     if (m->degree > 1) {
          printf("%c^%d", p.variable, m->degree);
     }
     else if (m->degree == 1) {
          printf("%c", p.variable);
     }
     m = m->next;

     /* iterate through other monomials */
     while (m->coeff.type != special) {
          /* sign */
          if ((coef_neg(m->coeff) || flip) && !(coef_neg(m->coeff) && flip)) {
               flipt = !flip;
               printf(" - ");
          }
          else {
               printf(" + ");
          }
          
          /* don't print unit coefficient, unless we won't print variable */
          if (!coef_one2(m->coeff) || m->degree == 0) {
               print_coefficient2(m->coeff, flipt);
          }
          /* coefficient and variable printed? */
          if (!coef_one2(m->coeff) && m->degree != 0) {
               printf("*");
          }
          /* print variable if degree > 0 */
          if (m->degree > 1) {
               printf("%c^%d", p.variable, m->degree);
          }
          else if (m->degree == 1) {
               printf("%c", p.variable);
          }
          m = m->next;
     }
}

void print_poly_sign(Polynomial p, int flip)
{
     print_coef_sign(poly_lc(p, p.variable), flip);
}

void print_poly_LaTeX(Polynomial p)
{
     MonoPtr m = p.head->next;
     int flipt = 0;
     /* check for zero polynomial */
     if (m->coeff.type == special) {
          printf("zero");
          return;
     }
     /* if (poly_neg(p)) { */
     /*      flip = !flip; */
     /* } */

     /* print first monomial */
     /* sign */
     if (coef_neg(m->coeff)) {
          flipt = 1;
          printf("-");
     }
     /* don't print unit coefficient, unless we won't print variable */
     if (!coef_one2(m->coeff) || m->degree == 0) {
          /* brackets around coefficient? */
          if (coef_num_monomials(m->coeff) > 1 && m->degree > 0) {
               printf("(");
               print_coefficient_LaTeX2(m->coeff, flipt);
               printf(")");
          }
          else {
               print_coefficient_LaTeX2(m->coeff, flipt);
          }
     }
     /* print variable if degree > 0 */
     if (m->degree > 1) {
          printf("%c^{%d}", p.variable, m->degree);
     }
     else if (m->degree == 1) {
          printf("%c", p.variable);
     }
     m = m->next;

     /* iterate through other monomials */
     while (m->coeff.type != special) {
          /* sign */
          if (coef_neg(m->coeff)) {
               flipt = 1;
               printf(" - ");
          }
          else {
               flipt = 0;
               printf(" + ");
          }
          /* don't print unit coefficient, unless we won't print variable */
          if (!coef_one2(m->coeff) || m->degree == 0) {
               /* brackets around coefficient? */
               if (coef_num_monomials(m->coeff) > 1 && m->degree > 0) {
                    printf("(");
                    print_coefficient_LaTeX2(m->coeff, flipt);
                    printf(")");
               }
               else {
                    print_coefficient_LaTeX2(m->coeff, flipt);
               }
          }
          /* print variable if degree > 0 */
          if (m->degree > 1) {
               printf("%c^{%d}", p.variable, m->degree);
          }
          else if (m->degree == 1) {
               printf("%c", p.variable);
          }
          m = m->next;
     }
}

void print_poly_LaTeX2(Polynomial p, int flip)
{
     MonoPtr m = p.head->next;
     int flipt = flip;
     /* check for zero polynomial */
     if (m->coeff.type == special) {
          printf("zero");
          return;
     }

     /* print first monomial */
     /* don't print unit coefficient, unless we won't print variable */
     if (!coef_one2(m->coeff) || m->degree == 0) {
          /* brackets around coefficient? */
          if (coef_num_monomials(m->coeff) > 1 && m->degree > 0) {
               printf("(");
               print_coefficient_LaTeX2(m->coeff, flipt);
               printf(")");
          }
          else {
               print_coefficient_LaTeX2(m->coeff, flipt);
          }
     }
     /* print variable if degree > 0 */
     if (m->degree > 1) {
          printf("%c^{%d}", p.variable, m->degree);
     }
     else if (m->degree == 1) {
          printf("%c", p.variable);
     }
     m = m->next;

     /* iterate through other monomials */
     while (m->coeff.type != special) {
          /* sign */
          if ((coef_neg(m->coeff) || flip) && !(coef_neg(m->coeff) && flip)) {
               flipt = !flip;
               printf(" - ");
          }
          else {
               printf(" + ");
          }
          /* don't print unit coefficient, unless we won't print variable */
          if (!coef_one2(m->coeff) || m->degree == 0) {
               /* brackets around coefficient? */
               if (coef_num_monomials(m->coeff) > 1 && m->degree > 0) {
                    printf("(");
                    print_coefficient_LaTeX2(m->coeff, flipt);
                    printf(")");
               }
               else {
                    print_coefficient_LaTeX2(m->coeff, flipt);
               }
          }
          /* print variable if degree > 0 */
          if (m->degree > 1) {
               printf("%c^{%d}", p.variable, m->degree);
          }
          else if (m->degree == 1) {
               printf("%c", p.variable);
          }
          m = m->next;
     }
}

/* void print_coefficient_simple(Coefficient c) */
/* { */
/*      switch (c.type) { */
/*      case rational: */
/*           if (bn_one(c.u.rat.den)) { */
/*                print_bigrat(c.u.rat); */
/*           } */
/*           else { */
/*                printf("("); */
/*                print_bigrat(c.u.rat); */
/*                printf(")"); */
/*           } */
/*           break; */
/*      case polynomial: */
/*           printf("("); */
/*           print_poly_simple(c.u.poly); */
/*           printf(")"); */
/*           break; */
/*      default: */
/*           break; */
/*      } */
/* } */

/* void print_coefficient_nonpretty(Coefficient c) */
/* { */
/*      switch (c.type) { */
/*      case rational: */
/*           if (bn_one(c.u.rat.den)) { */
/*                print_bigrat(c.u.rat); */
/*           } */
/*           else { */
/*                printf("("); */
/*                print_bigrat(c.u.rat); */
/*                printf(")"); */
/*           } */
/*           break; */
/*      case polynomial: */
/*           printf("("); */
/*           print_poly_nonpretty(c.u.poly); */
/*           printf(")"); */
/*           break; */
/*      default: */
/*           break; */
/*      } */
/* } */

void print_coefficient(Coefficient c)
{
     switch (c.type) {
     case rational:
          if (bn_one(c.u.rat.den)) {
               print_bigrat(c.u.rat);
          }
          else {
               printf("(");
               print_bigrat(c.u.rat);
               printf(")");
          }
          break;
     case polynomial:
          printf("(");
          print_poly(c.u.poly);
          printf(")");
          break;
     default:
          break;
     }
}

void print_coefficient2(Coefficient c, int flip)
{
     switch (c.type) {
     case rational:
          if (bn_one(c.u.rat.den)) {
               print_bigrat2(c.u.rat);
          }
          else {
               printf("(");
               print_bigrat2(c.u.rat);
               printf(")");
          }
          break;
     case polynomial:
          printf("(");
          print_poly2(c.u.poly, flip);
          printf(")");
          break;
     default:
          break;
     }
}

void print_coefficient_LaTeX(Coefficient c)
{
     switch (c.type) {
     case rational:
          print_bigrat_LaTeX(c.u.rat);
          break;
     case polynomial:
          print_poly_LaTeX(c.u.poly);
          break;
     default:
          break;
     }
}

void print_coefficient_LaTeX2(Coefficient c, int flip)
{
     switch (c.type) {
     case rational:
          print_bigrat_LaTeX2(c.u.rat);
          break;
     case polynomial:
          print_poly_LaTeX2(c.u.poly, flip);
          break;
     default:
          break;
     }
}

/* void print_coefficient3(Coefficient c) */
/* { */
/*      switch (c.type) { */
/*      case rational: */
/*           if (bn_one(c.u.rat.den)) { */
/*                print_bigrat3(c.u.rat); */
/*           } */
/*           else { */
/*                printf("("); */
/*                print_bigrat3(c.u.rat); */
/*                printf(")"); */
/*           } */
/*           break; */
/*      case polynomial: */
/*           printf("("); */
/*           print_poly3(c.u.poly); */
/*           printf(")"); */
/*           break; */
/*      default: */
/*           break; */
/*      } */
/* } */

int print_coef_sign(Coefficient c, int flip)
{
     switch (c.type) {
     case rational:
          print_br_sign(c.u.rat, flip);
          break;
     case polynomial:
          print_poly_sign(c.u.poly, flip);
          break;
     default:
          break;
     }
     return 1;
}

void copy_coefficient(Coefficient *c, Coefficient s)
{
     Coefficient old_coef = *c;
     
     c->type = s.type;
     switch (s.type) {
     case rational:
          init_bigrat(&c->u.rat);
          bigrat_copy(&c->u.rat, s.u.rat);
          break;
     case polynomial:
          c->u.poly.head = NULL;
          copy_poly(&c->u.poly, s.u.poly);
          break;
     default:
          break;
     }

     free_coefficient(&old_coef);
}

void free_coefficient(Coefficient *c)
{
     switch (c->type) {
     case rational:
          free_bigrat(&c->u.rat);
          break;
     case polynomial:
          free_poly(&c->u.poly);
          break;
     default:
          break;
     }
}

int coef_zero(Coefficient c)
{
     switch (c.type) {
     case rational:
          return br_zero(c.u.rat);
     case polynomial:
          if (c.u.poly.head->next->coeff.type == special) {
               return 1;
          }
          else {
               return 0;
          }
     default:
          return 0;
     }
}

int coef_one(Coefficient c)
{
     switch (c.type) {
     case rational:
          return br_one(c.u.rat);
     case polynomial:
          return poly_one(c.u.poly);
     default:
          return 0;
     }
}

int coef_one2(Coefficient c)
{
     switch (c.type) {
     case rational:
          return br_one2(c.u.rat);
     case polynomial:
          return poly_one(c.u.poly);
     default:
          return 0;
     }
}

int coef_deg(Coefficient c, char var)
{
     switch (c.type) {
     case rational:
          return 0;
     case polynomial:
          return poly_deg2(c.u.poly, var);
     default:
          return 0;
     }
}

void negate_coefficient(Coefficient *c)
{
     switch (c->type) {
     case rational:
          negate_bigrat(&c->u.rat);
          break;
     case polynomial:
          negate_polynomial(&c->u.poly);
          break;
     default:
          break;
     }
}

void add_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &add_bigrats);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          add_polynomials(&res->u.poly, left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          add_poly_rat(&res->u.poly, left.u.poly, right.u.rat);
     }
     else if (left.type == rational && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          add_poly_rat(&res->u.poly, right.u.poly, left.u.rat);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &sub_bigrats);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          sub_polynomials(&res->u.poly, left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          sub_poly_rat(&res->u.poly, left.u.poly, right.u.rat);
     }
     else if (left.type == rational && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          sub_poly_rat(&res->u.poly, right.u.poly, left.u.rat);
          negate_coefficient(res);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void mul_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &mul_bigrats);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          mul_polynomials(&res->u.poly, left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          mul_poly_rat(&res->u.poly, left.u.poly, right.u.rat);
     }
     else if (left.type == rational && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          mul_poly_rat(&res->u.poly, right.u.poly, left.u.rat);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void mul_coefficients2(Coefficient *res, Coefficient left, SHORT_INT_T right)
{
     Coefficient old_res = *res;
     BigRat temp;
     if (left.type == rational) {
          res->type = rational;
          res->u.rat.num = NULL;
          res->u.rat.den = NULL;
          mul_bigrats2(&res->u.rat, left.u.rat, right);
     }
     else if (left.type == polynomial) {
          temp = make_bigrat3(right);
          res->type = polynomial;
          res->u.poly.head = NULL;
          mul_poly_rat(&res->u.poly, left.u.poly, temp);
          free_bigrat(&temp);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void exact_div_coefficients(Coefficient *res,
                            Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &div_bigrats);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          exact_div_polynomials(&res->u.poly, left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          div_poly_rat(&res->u.poly, left.u.poly, right.u.rat);
     }
     else if (left.type == rational && right.type == polynomial) {
          if (coef_zero(left)) {
               /* everything divides zero */
               res->type = rational;
               res->u.rat = make_bigrat3(0);
          }
          else {
               printf("Error: division was not exact: rat/poly\n");
          }
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void exact_div_coefficients2(Coefficient *res,
                             Coefficient left, SHORT_INT_T right)
{
     Coefficient old_res = *res;
     BigRat t = {NULL, NULL};
     
     if (left.type == rational) {
          res->type = rational;
          res->u.rat.num = NULL;
          res->u.rat.den = NULL;
          div_bigrats2(&res->u.rat, left.u.rat, right);
     }
     else if (left.type == polynomial) {
          t = make_bigrat3(right);
          res->type = polynomial;
          res->u.poly.head = NULL;
          div_poly_rat(&res->u.poly, left.u.poly, t);
          free_bigrat(&t);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void polydiv_coefficients(Coefficient *q, Coefficient *r,
                          Coefficient left, Coefficient right)
{
     Coefficient old_q = *q;
     Coefficient old_r = *r;
     if (left.type == rational && right.type == rational) {
          q->type = rational;
          q->u.rat.num = NULL;
          q->u.rat.den = NULL;
          rat_coef_op(q, left, right, &div_bigrats);
          r->type = rational;
          r->u.rat = make_bigrat3(0);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          q->type = polynomial;
          q->u.poly.head = NULL;
          r->type = polynomial;
          r->u.poly.head = NULL;
          div_polynomials(&q->u.poly, &r->u.poly, left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          q->type = polynomial;
          q->u.poly.head = NULL;
          div_poly_rat(&q->u.poly, left.u.poly, right.u.rat);
          r->type = rational;
          r->u.rat = make_bigrat3(0);
     }
     else if (left.type == rational && right.type == polynomial) {
          q->type = rational;
          q->u.rat = make_bigrat3(0);
          r->type = special;
          copy_coefficient(r, left);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(q);
     coef_const_canonicalise(r);
     free_coefficient(&old_q);
     free_coefficient(&old_r);
}

void pseudo_div_coefficients(Coefficient *q, Coefficient *r,
                             Coefficient left, Coefficient right)
{
     Coefficient old_q = *q;
     Coefficient old_r = *r;
     Coefficient t = {special};
     char var;
     if (left.type == rational && right.type == rational) {
          /* Q=A, R=0 */
          q->type = special;
          copy_coefficient(q, left);
          r->type = rational;
          r->u.rat = make_bigrat3(0);
     }
     else if (left.type == polynomial && right.type == polynomial) {
          q->type = polynomial;
          q->u.poly.head = NULL;
          r->type = polynomial;
          r->u.poly.head = NULL;
          pseudo_div_polynomials(&q->u.poly, &r->u.poly,
                                 left.u.poly, right.u.poly);
     }
     else if (left.type == polynomial && right.type == rational) {
          /* Q=A*B^deg(A) */
          var = left.u.poly.variable;
          q->type = special;
          coef_power(&t, right, coef_deg(left, var));
          mul_coefficients(q, left, t);
          free_coefficient(&t);
          r->type = rational;
          r->u.rat = make_bigrat3(0);
     }
     else if (left.type == rational && right.type == polynomial) {
          q->type = rational;
          q->u.rat = make_bigrat3(0);
          r->type = special;
          copy_coefficient(r, left);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(q);
     coef_const_canonicalise(r);
     free_coefficient(&old_q);
     free_coefficient(&old_r);
}

void coef_power(Coefficient *res, Coefficient coef, SHORT_INT_T power)
{
     Coefficient old_res = *res;
     if (coef.type == rational) {
          res->type = rational;
          res->u.rat.num = NULL;
          res->u.rat.den = NULL;
          br_power(&res->u.rat, coef.u.rat, power);
     }
     else if (coef.type == polynomial) {
          res->type = polynomial;
          res->u.poly.head = NULL;
          poly_power(&res->u.poly, coef.u.poly, power);
     }
     else {
          printf("Error! Coefficient has type special!\n");
          exit(1);
     }
     coef_const_canonicalise(res);
     free_coefficient(&old_res);
}

void coef_gcd(Coefficient *res, Coefficient a, Coefficient b)
{
     Coefficient old_res = *res;
     Coefficient ac = {special}, bc = {special}, app = {special}, bpp = {special};
     Coefficient ppgcd = {special}, cgcd = {special};
     char var;

     /* printf("****coef GCD ****\n"); */
     /* PRINTC(a); */
     /* PRINTC(b); */
     /* printf("var: %c\n", var); */

     if (a.type == rational && b.type == rational) {
          /* rationals */
          /* printf("doing rat gcd...\n"); */
          res->type = rational;
          res->u.rat = make_bigrat3(0);
          gcd(&res->u.rat.num, a.u.rat.num, b.u.rat.num);
          /* printf("rat gcd: "); print_bigrat(res->u.rat); printf("\n"); */
          /* printf("done rat gcd...\n"); */
     }
     else {
          /* TODO this is actually easier if only one is a polynomial
           * ie. gcd(pp,pp) = 1 so we only need gcd(cont,cont) */
          if (a.type == polynomial) {
               var = a.u.poly.variable;
               if (b.type == polynomial
                   && var_rank(var) > var_rank(b.u.poly.variable)) {
                    var = b.u.poly.variable;
               }
          }
          else {
               var = b.u.poly.variable;
          }
          
          coef_content(&ac, a, var);
          /* PRINTC(ac); */
          coef_content(&bc, b, var);
          /* PRINTC(bc); */
          exact_div_coefficients(&app, a, ac);
          /* PRINTC(app); */
          exact_div_coefficients(&bpp, b, bc);
          /* PRINTC(bpp); */

          coef_gcd(&cgcd, ac, bc);
          SubResultantGCD(&ppgcd, app, bpp, var);
          res->type = special;
          mul_coefficients(res, cgcd, ppgcd);
     }

     /* printf("****coef GCD answer:\n"); */
     /* PRINTC(*res); */

     free_coefficient(&old_res);
     free_coefficient(&ac);
     free_coefficient(&bc);
     free_coefficient(&app);
     free_coefficient(&bpp);
     free_coefficient(&ppgcd);
     free_coefficient(&cgcd);
}

int coef_neg(Coefficient c)
{
     switch (c.type) {
     case rational:
          return br_neg(c.u.rat);
     case polynomial:
          return poly_neg(c.u.poly);
     default:
          return 0;
     }
}

BigRat coef_rat_part(Coefficient c)
{
     BigRat r = {NULL, NULL}, t = {NULL, NULL};
     BigRat t1 = {NULL, NULL}, t2 = {NULL, NULL};
     MonoPtr p, q;
     
     switch (c.type) {
     case rational:
          return make_bigrat2(c.u.rat.den);
     case polynomial:
          p = c.u.poly.head->next;
          q = p->next;
          if (p->coeff.type == special) { /* zero */
               return make_bigrat3(0);
          }
          if (q->coeff.type == special) { /* one coefficient */
               return coef_rat_part(p->coeff);
          }
          /* at least two coefficients */
          init_bigrat(&r);
          t1 = coef_rat_part(p->coeff);
          t2 = coef_rat_part(q->coeff);
          gcd(&t.num, t1.num, t2.num);
          mul_bignums(&r.num, t1.num, t2.num);
          div_bignums(&r.num, &t.den, r.num, t.num);
          free_bigrat(&t1);
          free_bigrat(&t2);
          for (p = q->next; p->coeff.type != special; p = p->next) {
               t1 = coef_rat_part(p->coeff);
               gcd(&t.num, r.num, t1.num);
               mul_bignums(&r.num, r.num, t1.num);
               free_bigrat(&t1);
               div_bignums(&r.num, &t.den, r.num, t.num);
          }
          free_bigrat(&t);
          return r;
     default:
          return r;
     }
}

void coef_const_canonicalise(Coefficient *c)
{
     Coefficient t;
     
     if (c->type == rational) {
          return;
     }

     if (poly_deg(c->u.poly) > 0) {
          return;
     }

     if (poly_zero(c->u.poly)) {
          free_coefficient(c);
          c->type = rational;
          c->u.rat = make_bigrat3(0);
          return;
     }

     t = poly_lc(c->u.poly, c->u.poly.variable);
     c->u.poly.head->next->coeff.type = special;
     free_poly(&c->u.poly);
     *c = t;
}

void coef_differentiate(Coefficient *cd, Coefficient c, char var)
{
     Coefficient old_cd;
     
     switch (c.type)  {
     case rational:
          free_coefficient(cd);
          cd->type = rational;
          cd->u.rat = make_bigrat3(0);
          break;

     case polynomial:
          old_cd = *cd;
          cd->type = polynomial;
          cd->u.poly.head = NULL;
          poly_differentiate(&cd->u.poly, c.u.poly, var);
          free_coefficient(&old_cd);
          break;

     default:
          break;
     }
     coef_const_canonicalise(cd);
}

void coef_integrate(Coefficient *ci, Coefficient c, char var)
{
     Coefficient old_ci = *ci;
     
     switch (c.type)  {
     case rational:
          ci->type = polynomial;
          ci->u.poly = make_mono_poly(var, 1);
          mul_coefficients(ci, *ci, c);
          break;

     case polynomial:
          ci->type = polynomial;
          ci->u.poly.head = NULL;
          poly_integrate(&ci->u.poly, c.u.poly, var);
          break;

     default:
          break;
     }
     coef_const_canonicalise(ci);

     free_coefficient(&old_ci);
}

void subst_var_coef(Coefficient *c, Coefficient sol, char var)
{
     /* only care about polynomial case */
     switch (c->type) {
     case polynomial:
          subst_var_poly(&c->u.poly, sol, var);
          break;
     default:
          break;
     }
     coef_const_canonicalise(c);
}

void coef_content(Coefficient *cont, Coefficient p, char var)
{
     /* cont(0)==pp(0)==0 by convention */
     if (coef_zero(p)) {
          free_coefficient(cont);
          cont->type = rational;
          cont->u.rat = make_bigrat3(0);
          return;
     }
          
     switch (p.type) {
     case rational:
          copy_coefficient(cont, p);
          break;

     case polynomial:
          poly_content(cont, p.u.poly, var);
          break;

     default:
          break;
     }

     /* printf("cont of "); PRINTC(p); */
     /* printf(" = "); PRINTC(*cont); printf("\n"); */
}

void coef_pp(Coefficient *pp, Coefficient p, char var)
{
     Coefficient old_pp;

     /* cont(0)==pp(0)==0 by convention */
     if (coef_zero(p)) {
          free_coefficient(pp);
          pp->type = rational;
          pp->u.rat = make_bigrat3(0);
          return;
     }
     
     switch (p.type) {
     case rational:
          free_coefficient(pp);
          pp->type = rational;
          pp->u.rat = make_bigrat3(1);
          break;

     case polynomial:
          old_pp = *pp;
          pp->type = polynomial;
          pp->u.poly.head = NULL;
          poly_pp(&pp->u.poly, p.u.poly, var);
          coef_const_canonicalise(pp);
          free_coefficient(&old_pp);
          break;

     default:
          break;
     }
}

const Coefficient coef_lc(Coefficient c, char var)
{
     Coefficient f = {special};
     
     switch (c.type) {
     case rational:
          return c;

     case polynomial:
          return poly_lc(c.u.poly, var);

     default:
          return f;
     }
}

unsigned coef_num_monomials(Coefficient c)
{
     switch (c.type) {
     case rational:
          return 1;

     case polynomial:
          return poly_num_monomials(c.u.poly);

     default:
          return 0;
     }
}

int poly_zero(Polynomial p)
{
     if (p.head->next->coeff.type == special) {
          return 1;
     }
     return 0;
}

int poly_one(Polynomial p)
{
     if (p.head->next->coeff.type != special
         && p.head->next->next->coeff.type == special
         && p.head->next->degree == 0
         && coef_one(p.head->next->coeff)) {
          return 1;
     }
     return 0;
}

int poly_one2(Polynomial p)
{
     if (p.head->next->coeff.type != special
         && p.head->next->next->coeff.type == special
         && p.head->next->degree == 0
         && coef_one2(p.head->next->coeff)) {
          return 1;
     }
     return 0;
}

int poly_deg(Polynomial p)
{
     return p.head->next->degree;
}

int poly_deg2(Polynomial p, char var)
{
     if (var == p.variable) {
          return p.head->next->degree;
     }
     else if (var_rank(var) < var_rank(p.variable)) {
          return 0;
     }
     else {
          /* TODO */
          printf("Error! Trying to get deg in higher ranking variable!\n");
          return p.head->next->degree;
     }
}

const Coefficient poly_lc(Polynomial p, char var)
{
     Coefficient c;
     if (var == p.variable) {
          return p.head->next->coeff;
     }
     else if (var_rank(var) < var_rank(p.variable)) {
          c.type = polynomial;
          c.u.poly = p;
          return c;
     }
     else {
          printf("Error! Trying to get lc in higher ranking variable!\n");
          return p.head->next->coeff;
     }
}

unsigned poly_num_monomials(Polynomial p)
{
     MonoPtr q;
     unsigned n = 0;
     for (q = p.head->next; q->coeff.type != special; ++n, q = q->next);

     return n;
}

void add_monomial(Polynomial *p, int degree, Coefficient coef)
{
     MonoPtr q, q1;             /* q1 will be one step behind q */

     if (coef_zero(coef)) {
          return;
     }
     /* find position to add monomial */
     q1 = p->head;
     q = q1->next;
     while (q->coeff.type != special && q->degree > degree) {
          /* move along */
          q1 = q;
          q = q1->next;
     }
     /* is there already a monomial of this degree? */
     if (q->degree == degree && q->coeff.type != special) {
          add_coefficients(&q->coeff, q->coeff, coef);
          /* remove zero coefficient */
          if (coef_zero(q->coeff)) {
               q1->next = q->next;
               free_coefficient(&q->coeff);
               free(q);
               q = NULL;
          }
     }
     else {
          /* insert between q1 and q */
          q1->next = malloc(sizeof(Monomial));
          q1->next->next = q;
          /* initialise new monomial */
          q1->next->degree = degree;
          q1->next->coeff.type = coef.type;
          switch (coef.type) {
          case rational:
               init_bigrat(&q1->next->coeff.u.rat);
               bigrat_copy(&q1->next->coeff.u.rat, coef.u.rat);
               break;
          case polynomial:
               q1->next->coeff.u.poly = make_zero_poly(coef.u.poly.variable);
               copy_poly(&q1->next->coeff.u.poly, coef.u.poly);
               break;
          default:
               break;
          }
     }
}

void sub_monomial(Polynomial* p, int degree, Coefficient coef)
{
     MonoPtr q, q1;             /* q1 will be one step behind q */
     
     if (coef_zero(coef)) {
          return;
     }
     /* find position to add monomial */
     q1 = p->head;
     q = q1->next;
     while (q->coeff.type != special && q->degree > degree) {
          /* move along */
          q1 = q;
          q = q1->next;
     }
     /* is there already a monomial of this degree? */
     if (q->degree == degree && q->coeff.type != special) {
          sub_coefficients(&q->coeff, q->coeff, coef);
          /* remove zero coefficient */
          if (coef_zero(q->coeff)) {
               q1->next = q->next;
               free_coefficient(&q->coeff);
               free(q);
               q = NULL;
          }
     }
     else {
          /* insert new (nonzero) coefficient between q1 and q */
          q1->next = malloc(sizeof(Monomial));
          q1->next->next = q;
          /* initialise new monomial */
          q1->next->degree = degree;
          q1->next->coeff.type = coef.type;
          switch (coef.type) {
          case rational:
               init_bigrat(&q1->next->coeff.u.rat);
               bigrat_copy(&q1->next->coeff.u.rat, coef.u.rat);
               negate_bigrat(&q1->next->coeff.u.rat);
               break;
          case polynomial:
               q1->next->coeff.u.poly = make_zero_poly(coef.u.poly.variable);
               copy_poly(&q1->next->coeff.u.poly, coef.u.poly);
               negate_polynomial(&q1->next->coeff.u.poly);
               break;
          default:
               break;
          }
     }
}

void negate_polynomial(Polynomial *p)
{
     /* negate all coefficients */
     MonoPtr q;

     for (q = p->head->next; q->coeff.type != special; q = q->next) {
          negate_coefficient(&q->coeff);
     }
}

void add_polynomials(Polynomial *res, Polynomial left, Polynomial right)
{
     Polynomial old_res;
     MonoPtr p;
     Coefficient t;

     old_res = *res;

     /* different variables */
     if (var_rank(left.variable) < var_rank(right.variable)) {
          res->head = NULL;
          copy_poly(res, left);
          t.type = polynomial;
          t.u.poly = right;
          add_monomial(res, 0, t);
          free_poly(&old_res);
          return;
     }
     else if (var_rank(right.variable) < var_rank(left.variable)) {
          res->head = NULL;
          copy_poly(res, right);
          t.type = polynomial;
          t.u.poly = left;
          add_monomial(res, 0, t);
          free_poly(&old_res);
          return;
     }

     /* same variable */
     *res = make_zero_poly(left.variable);

     for (p = left.head->next; p->coeff.type != special; p = p->next) {
          add_monomial(res, p->degree, p->coeff);
     }
     for (p = right.head->next; p->coeff.type != special; p = p->next) {
          add_monomial(res, p->degree, p->coeff);
     }

     /* free old result */
     free_poly(&old_res);
}

void sub_polynomials(Polynomial *res, Polynomial left, Polynomial right)
{
     Polynomial old_res;
     MonoPtr p;
     Coefficient t;

     old_res = *res;

     /* different variables */
     if (var_rank(left.variable) < var_rank(right.variable)) {
          res->head = NULL;
          copy_poly(res, left);
          t.type = polynomial;
          t.u.poly = right;
          sub_monomial(res, 0, t);
          free_poly(&old_res);          
          return;
     }
     else if (var_rank(right.variable) < var_rank(left.variable)) {
          res->head = NULL;
          copy_poly(res, right);
          t.type = polynomial;
          t.u.poly = left;
          sub_monomial(res, 0, t);
          free_poly(&old_res);          
          return;
     }

     /* same variable */
     *res = make_zero_poly(left.variable);

     for (p = left.head->next; p->coeff.type != special; p = p->next) {
          add_monomial(res, p->degree, p->coeff);
     }
     for (p = right.head->next; p->coeff.type != special; p = p->next) {
          sub_monomial(res, p->degree, p->coeff);
     }

     /* free old result */
     free_poly(&old_res);
}

void mul_polynomials(Polynomial *res, Polynomial left, Polynomial right)
{
     Polynomial old_res;
     MonoPtr p, q;
     Coefficient t;
     t.type = special;

     old_res = *res;

     if (poly_zero(left) || poly_zero(right)) {
          *res = make_zero_poly(left.variable);
          free_poly(&old_res);
          return;
     }

     /* different variables */
     if (var_rank(left.variable) < var_rank(right.variable)) {
          res->head = NULL;
          copy_poly(res, left);
          t.type = polynomial;
          t.u.poly = right;

          for (p = res->head->next; p->coeff.type != special; p = p->next) {
               mul_coefficients(&p->coeff, p->coeff, t);
          }
          free_poly(&old_res);
          return;
     }
     else if (var_rank(right.variable) < var_rank(left.variable)) {
          res->head = NULL;
          copy_poly(res, right);
          t.type = polynomial;
          t.u.poly = left;

          for (p = res->head->next; p->coeff.type != special; p = p->next) {
               mul_coefficients(&p->coeff, p->coeff, t);
          }
          free_poly(&old_res);
          return;
     }

     /* polynomials of same variable */

     *res = make_zero_poly(left.variable);

     for (p = left.head->next; p->coeff.type != special; p = p->next) {
          for (q = right.head->next; q->coeff.type != special; q = q->next) {
               mul_coefficients(&t, p->coeff, q->coeff);
               add_monomial(res, p->degree + q->degree, t);
          }
     }

     free_coefficient(&t);
     /* free old result */
     free_poly(&old_res);
}

/* standard polynomial division for coefficients in a field (rationals) */
void div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A, Polynomial B)
{
     Polynomial old_Q, old_R, T, BT;
     Coefficient tc, tr;
     int s_degree;
     MonoPtr t;
     char var;

     if (poly_zero(B)) {
          printf("Error! Polynomial division by zero!\n");
          return;
     }

     old_Q = *Q;
     old_R = *R;

     if (poly_zero(A)) {
          *Q = make_zero_poly(A.variable);
          *R = make_zero_poly(A.variable);
          free_poly(&old_Q);
          free_poly(&old_R);
     }

     tr.type = special;

     /* different variables */
     if (var_rank(A.variable) < var_rank(B.variable)) {
          /* this might divide - divide each coefficient of the LHS by the
           * RHS, if one or more does not divide then the whole thing does not
           * divide */
          T.head = NULL;
          copy_poly(&T, A);
          tc.type = polynomial;
          tc.u.poly = B;

          for (t = T.head->next; t->coeff.type != special; t = t->next) {
               polydiv_coefficients(&t->coeff, &tr, t->coeff, tc);
               if (!coef_zero(tr)) {
                    /* doesn't divide */
                    *Q = make_zero_poly(A.variable);
                    free_poly(&old_Q);
                    copy_poly(R, A);
                    return;
               }
          }
          /* divided */
          copy_poly(Q, T);
          *R = make_zero_poly(A.variable);
          free_poly(&old_R);
          return;
     }
     else if (var_rank(B.variable) < var_rank(A.variable)) {
          /* this definitely does not divide */
          *Q = make_zero_poly('x');
          copy_poly(R, A);
          free_poly(&old_Q);
          return;
     }

     /* same variables */
     var = A.variable;
     
     T = make_zero_poly(A.variable);
     BT = make_zero_poly(A.variable);
     /* setup T, it will contain only one monomial */
     T.head->next = malloc(sizeof(Monomial));
     T.head->next->next = T.head;
     t = T.head->next;
     t->degree = 0;
     t->coeff.type = rational;
     t->coeff.u.rat.num = NULL;
     t->coeff.u.rat.den = NULL;
     
     *Q = make_zero_poly(A.variable);
     copy_poly(R, A);
     while (!poly_zero(*R) && (s_degree = poly_deg(*R)-poly_deg(B)) >= 0) {
          t->degree = s_degree;
          polydiv_coefficients(&t->coeff, &tr, poly_lc(*R, var), poly_lc(B, var));
          if (!coef_zero(tr)) {
               /* doesn't divide */
               free_poly(Q);
               *Q = make_zero_poly(A.variable);
               free_poly(&old_Q);
               copy_poly(R, A);
               return;
          }    
          add_monomial(Q, s_degree, T.head->next->coeff);
          mul_polynomials(&BT, B, T);
          sub_polynomials(R, *R, BT);
     }

     free_poly(&old_Q);
     free_poly(&T);
     free_poly(&BT);
}

void pseudo_div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A,
                            Polynomial B)
{
     Polynomial old_Q, old_R, T, BT, b;
     int s_degree, N;
     MonoPtr t;

     if (poly_zero(B)) {
          printf("Error! Polynomial pseudo-division by zero!\n");
          return;
     }

     /* if (A.variable != B.variable) { */
     /*      printf("Warning, psuedo div in different variables!\n"); */
     /* } */

     old_Q = *Q;
     Q->head = NULL;
     old_R = *R;
     R->head = NULL;

     if (poly_zero(A)) {
          *Q = make_zero_poly(A.variable);
          *R = make_zero_poly(A.variable);
          free_poly(&old_Q);
          free_poly(&old_R);
     }

     /* different variables */
     if (var_rank(A.variable) < var_rank(B.variable)) {
          /* Q = A*B^deg(A), R = 0 */
          T.head = NULL;
          poly_power(&T, B, poly_deg(A));
          mul_polynomials(Q, A, T);
          *R = make_zero_poly(A.variable);
          free_poly(&T);
          free_poly(&old_R);
          return;
     }
     else if (var_rank(B.variable) < var_rank(A.variable)) {
          /* this definitely does not divide */
          *Q = make_zero_poly('x');
          copy_poly(R, A);
          free_poly(&old_Q);
          return;
     }

     /* same variables */
     if (poly_deg(B) > poly_deg(A)) {
          /* won't divide so Q = 0, R = A */
          *Q = make_zero_poly(A.variable);
          copy_poly(R, A);
          free_poly(&old_Q);
          return;
     }

     T = make_zero_poly(A.variable);
     BT = make_zero_poly(A.variable);
     /* setup T, it will contain only one monomial */
     T.head->next = malloc(sizeof(Monomial));
     T.head->next->next = T.head;
     t = T.head->next;
     t->degree = 0;
     t->coeff.type = rational;
     t->coeff.u.rat.num = NULL;
     t->coeff.u.rat.den = NULL;

     b = make_zero_poly(A.variable);
     add_monomial(&b, 0, poly_lc(B, A.variable));

     N = poly_deg(A) - poly_deg2(B, A.variable) + 1;
     
     *Q = make_zero_poly(A.variable);
     copy_poly(R, A);
     while (!poly_zero(*R)
            && (s_degree = poly_deg2(*R, A.variable)-poly_deg2(B, A.variable))
            >= 0) {
          /* printf("pseudo div: %d\n", s_degree); */
          /* print_poly(*R); */
          /* printf("\n"); */
          t->degree = s_degree;
          copy_coefficient(&t->coeff, poly_lc(*R, A.variable));
          --N;
          mul_polynomials(Q, *Q, b);
          add_monomial(Q, s_degree, T.head->next->coeff);
          mul_polynomials(R, *R, b);
          mul_polynomials(&BT, B, T);
          sub_polynomials(R, *R, BT);
     }

     poly_power(&b, b, abs(N));
     if (N > 0) {
          mul_polynomials(Q, *Q, b);
          mul_polynomials(R, *R, b);
     }
     else if (N < 0) {
          div_polynomials(Q, &T, *Q, b);
          div_polynomials(R, &T, *R, b);
     }

     free_poly(&old_Q);
     free_poly(&T);
     free_poly(&BT);
}

void exact_div_polynomials(Polynomial *Q, Polynomial A, Polynomial B)
{
     Polynomial R = make_zero_poly(A.variable);
     div_polynomials(Q, &R, A, B);
     if (!poly_zero(R)) {
          printf("Error! Exact division not exact!\n");
     }
     free_poly(&R);
}

void add_poly_rat(Polynomial *res, Polynomial left, BigRat right)
{
     Coefficient coef;
     
     coef.type = rational;
     /* hack because we know the rat will get copied by add_monomial() */
     coef.u.rat.num = right.num;
     coef.u.rat.den = right.den;

     copy_poly(res, left);
     add_monomial(res, 0, coef);
}

void sub_poly_rat(Polynomial *res, Polynomial left, BigRat right)
{
     Coefficient coef;
     
     coef.type = rational;
     /* hack because we know the rat will get copied by sub_monomial() */
     coef.u.rat.num = right.num;
     coef.u.rat.den = right.den;

     copy_poly(res, left);
     sub_monomial(res, 0, coef);
}

void mul_poly_rat(Polynomial *res, Polynomial left, BigRat right)
{
     Polynomial old_res = *res;
     Coefficient coef;
     MonoPtr q;

     if (br_zero(right)) {
          *res = make_zero_poly(left.variable);
          free_poly(&old_res);
          return;
     }
     
     coef.type = rational;
     /* hack because we know the rat will get copied by mul_coefficients() */
     coef.u.rat.num = right.num;
     coef.u.rat.den = right.den;

     copy_poly(res, left);

     /* multiply each coefficient by the rational */
     for (q = res->head->next; q->coeff.type != special; q = q->next) {
          mul_coefficients(&q->coeff, q->coeff, coef);
     }
}

void div_poly_rat(Polynomial *res, Polynomial left, BigRat right)
{
     Coefficient coef;
     MonoPtr q;
     
     coef.type = rational;
     /* hack because we know the rat will get copied by div_coefficients() */
     coef.u.rat.num = right.num;
     coef.u.rat.den = right.den;

     copy_poly(res, left);

     /* divide each coefficient by the rational */
     for (q = res->head->next; q->coeff.type != special; q = q->next) {
          exact_div_coefficients(&q->coeff, q->coeff, coef);
     }
}

void poly_power(Polynomial *res, Polynomial p, SHORT_INT_T power)
{
     Polynomial temp;
     SHORT_INT_T mask = 0;

     if (power == 0) {
          free_poly(res);
          *res = make_one_poly(p.variable);
          return;
     }
     
     mask = ~(~mask>>1);        /* MSB of short int */

     temp = make_zero_poly(p.variable);
     copy_poly(&temp, p);

     /* find first 1 in binary representation of power and skip it */
     while (!(mask & power) && mask) {
          mask = mask>>1;
     }
     mask = mask>>1;
     for (; mask; mask = mask>>1) {
          mul_polynomials(&temp, temp, temp);
          if (mask & power) {
               mul_polynomials(&temp, temp, p);
          }
     }

     copy_poly(res, temp);
     free_poly(&temp);
}

void poly_splice_add(Polynomial *left, Polynomial *right)
{
     MonoPtr q, q1, r, r1;

     if (left->variable != right->variable) {
          add_polynomials(left, *left, *right);
          return;
     }

     q1 = left->head;
     q = q1->next;

     r1 = right->head;
     r = r1->next;

     /* iterate through each monomial in right */
     while (r->coeff.type != special) {
          /* find place for this monomial in left */
          for (; q->degree > r->degree; q1 = q, q = q->next);
          /* printf("add: %d,%d\n",r->degree,q->degree); */
          if (r->degree == q->degree && q->coeff.type != special) {
               /* add coefficients */
               add_coefficients(&q->coeff, q->coeff, r->coeff);
               /* remove zero coefficient */
               if (coef_zero(q->coeff)) {
                    /* printf("removing: %d\n", q->degree); */
                    q1->next = q->next;
                    free_coefficient(&q->coeff);
                    free(q);
                    q = q1->next;
               }
               /* advance right pointer */
               r1 = r;
               r = r->next;
          }
          else {
               /* splice monomial */
               q1->next = r;
               q1 = r;
               r = r->next;
               r1->next = r;
               q1->next = q;
          }
     }
}

void poly_splice_sub(Polynomial *left, Polynomial *right)
{
     MonoPtr q, q1, r, r1;

     if (left->variable != right->variable) {
          sub_polynomials(left, *left, *right);
          return;
     }

     q1 = left->head;
     q = q1->next;

     r1 = right->head;
     r = r1->next;

     /* iterate through each monomial in right */
     while (r->coeff.type != special) {
          /* find place for this monomial in left */
          for (; q->degree > r->degree; q1 = q, q = q->next);
          /* printf("sub: %d,%d\n",r->degree,q->degree); */
          if (r->degree == q->degree && q->coeff.type != special) {
               /* sub coefficients */
               sub_coefficients(&q->coeff, q->coeff, r->coeff);

               /* remove zero coefficient */
               if (coef_zero(q->coeff)) {
                    /* printf("removing: %d\n", q->degree); */
                    q1->next = q->next;
                    free_coefficient(&q->coeff);
                    free(q);
                    q = q1->next;
               }
               /* advance right pointer */
               r1 = r;
               r = r->next;
          }
          else {
               negate_coefficient(&r->coeff);
               /* splice monomial */
               q1->next = r;
               q1 = r;
               r = r->next;
               r1->next = r;
               q1->next = q;
          }
     }
}

int poly_neg(Polynomial p)
{
     return coef_neg(poly_lc(p, p.variable));
}

void poly_content(Coefficient *cont, Polynomial p, char var)
{
     Coefficient old_res = *cont;

     MonoPtr q, r;

     /* cont(0)==pp(0)==0 by convention */
     if (poly_zero(p)) {
          cont->type = rational;
          cont->u.rat = make_bigrat3(0);
          free_coefficient(&old_res);
          return;
     }

     if (var_rank(var) < var_rank(p.variable)) {
          cont->type = polynomial;
          cont->u.poly.head = NULL;
          copy_poly(&cont->u.poly, p);
          free_coefficient(&old_res);
          return;
     }
     else if (var_rank(var) > var_rank(p.variable)) {
          /* TODO */
          printf("Error! Trying to get content in higher ranking variable!\n");
          exit(1);
     }

     q = p.head->next;
     r = q->next;
     if (r->coeff.type == special) {
          /* only one coefficient */
          copy_coefficient(cont, q->coeff);
          return;
     }

     coef_gcd(cont, q->coeff, r->coeff);

     /* TODO we could catch when the gcd is 1 and terminate early */
     for (q = r->next; q->coeff.type != special; q = q->next) {
          coef_gcd(cont, *cont, q->coeff);
     }

     if (poly_neg(p)) {
          negate_coefficient(cont);
     }

     free_coefficient(&old_res);
}

void poly_pp(Polynomial *pp, Polynomial p, char var)
{
     Coefficient content = {special};

     poly_content(&content, p, var);

     if (content.type == rational) {
          div_poly_rat(pp, p, content.u.rat);
     }
     else if (content.type == polynomial) {
          exact_div_polynomials(pp, p, content.u.poly);
     }

     free_coefficient(&content);
}

void poly_differentiate(Polynomial *pd, Polynomial p, char var)
{
     MonoPtr q, r;

     if (var_rank(var) < var_rank(p.variable)) {
          /* constant */
          free_poly(pd);
          *pd = make_zero_poly(p.variable);
          return;
     }
     else if (var_rank(var) > var_rank(p.variable)) {
          /* TODO */
          printf("Error! Trying to differentiate in higher ranking variable!\n");
          return;
     }

     copy_poly(pd, p);
     for (r = pd->head, q = r->next;
          q->coeff.type != special;
          r = q, q = q->next)
     {
          if (q->degree == 0) {
               /* remove monomial */
               r->next = q->next;
               free_coefficient(&q->coeff);
               free(q);
               q = r;
               continue;
          }
          /* reduce power by one and multiply by old power */
          mul_coefficients2(&q->coeff, q->coeff, q->degree--);
     }
}

void poly_integrate(Polynomial *pi, Polynomial p, char var)
{
     MonoPtr q;

     if (var_rank(var) < var_rank(p.variable)) {
          /* constant */
          *pi = make_mono_poly(var, 1);
          mul_polynomials(pi, *pi, p);
          return;
     }
     else if (var_rank(var) > var_rank(p.variable)) {
          /* TODO */
          printf("Error! Trying to differentiate in higher ranking variable!\n");
          return;
     }

     copy_poly(pi, p);
     for (q = pi->head->next; q->coeff.type != special; q = q->next) {
          /* increase power by one and divide by new power */
          exact_div_coefficients2(&q->coeff, q->coeff, ++q->degree);
     }
}

void solve_linear_poly(Coefficient *sol, Polynomial p)
{
     Coefficient t = {special};
     MonoPtr m, c;

     if (poly_deg(p) != 1) {
          printf("Error! Trying to solve non-linear poly!\n");
          return;
     }

     m = p.head->next;
     c = m->next;

     if (m->coeff.type != rational || c->coeff.type !=rational) {
          printf("Error! Solve only works for univariate polys!\n");
     }

     copy_coefficient(&t, c->coeff);
     negate_coefficient(&t);
     exact_div_coefficients(&t, t, m->coeff);

     copy_coefficient(sol, t);

     free_coefficient(&t);
}

void subst_var_poly(Polynomial *p, Coefficient sol, char var)
{
     MonoPtr q;
     Coefficient tsol = {special};
     Polynomial res;

     if (var_rank(var) < var_rank(p->variable)) {
          /* nothing to do since var can't exist in this poly */
          return;
     }
     else if (var_rank(var) > var_rank(p->variable)) {
          /* var might be in the coefficients */
          for (q = p->head->next; q->coeff.type != special; q = q->next) {
               subst_var_coef(&q->coeff, sol, var);
          }
          return;
     }

     /* this poly is in same var */
     res = make_zero_poly(var);

     for (q = p->head->next; q->coeff.type != special; q = q->next) {
          coef_power(&tsol, sol, q->degree);
          mul_coefficients(&tsol, tsol, q->coeff);
          add_monomial(&res, 0, tsol);
     }

     copy_poly(p, res);

     free_poly(&res);
     free_coefficient(&tsol);
}

int poly_univar(Polynomial p)
{
     MonoPtr q;

     for (q = p.head->next; q->coeff.type != special; q = q->next) {
          if (q->coeff.type == polynomial) {
               return 0;
          }
     }

     return 1;
}

BigRat poly_rat_part(Polynomial p)
{
     Coefficient c;
     c.type = polynomial;
     c.u.poly = p;
     return coef_rat_part(c);
}

/* common part of coefficient arithmetic for rationals */
static void rat_coef_op(Coefficient *res, Coefficient left, Coefficient right,
                        void (*op_fun)(BigRat*, BigRat, BigRat))
{
     res->type = rational;
     res->u.rat.num = NULL;
     res->u.rat.den = NULL;
     op_fun(&res->u.rat, left.u.rat, right.u.rat);
}
