#include <stdlib.h>
#include <stdio.h>

#include "polynomial.h"
#include "bigrat.h"

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

void free_poly(Polynomial *p)
{
     MonoPtr q, r;
     /* turn mono list into a normal list */
     q = p->head->next;
     p->head->next = NULL;
     for ( ; q != NULL; q=r) {
          r = q->next;
          free(q);
     }
     p->head = NULL;
}

void print_poly(Polynomial p)
{
     MonoPtr m = p.head->next;
     /* check for zero polynomial */
     if (m->coeff.type == special) {
          printf("zero");
          return;
     }
     /* iterate through monomials */
     do {
          print_coefficient(m->coeff);
          if (m->degree != 0 && m->degree != 1) {
               printf("%c^%d", p.variable, m->degree);
          }
          else if (m->degree == 1) {
               printf("%c", p.variable);
          }
          m = m->next;
     } while (m->coeff.type != special && printf(" + "));
}

void print_coefficient(Coefficient c)
{
     switch (c.type) {
     case rational:
          if (br_one(c.u.rat)) {
               break;
          }
          if (bn_one(c.u.rat.den)) {
               print_bigrat(c.u.rat);
          }
          else {
               printf("(");
               print_bigrat(c.u.rat);
               printf(")");
          }
          break;
     default:
          break;
     }
}

void free_coefficient(Coefficient *c)
{
     switch (c->type) {
     case rational:
          free_bigrat(&c->u.rat);
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
          if (c.u.poly->head->next->coeff.type == special) {
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
          /* poly_one() */
          return 0;
     default:
          return 0;
     }
}

void add_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     if (left.type == rational && right.type == rational) {
          add_bigrats(&res->u.rat, left.u.rat, right.u.rat);
     }
}

void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     if (left.type == rational && right.type == rational) {
          sub_bigrats(&res->u.rat, left.u.rat, right.u.rat);
     }
}

void mul_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     if (left.type == rational && right.type == rational) {
          mul_bigrats(&res->u.rat, left.u.rat, right.u.rat);
     }
}

void div_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     if (left.type == rational && right.type == rational) {
          div_bigrats(&res->u.rat, left.u.rat, right.u.rat);
     }
}

void add_monomial(Polynomial *p, int degree, Coefficient coef)
{
     MonoPtr q, q1;             /* q1 will be one step behind q */
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
     }
     else {
          /* insert between q1 and q */
          q1->next = malloc(sizeof(Monomial));
          q1->next->next = q;
          /* initialise new monomial */
          q1->next->degree = degree;
          /* copy ***RATIONAL ONLY*** TODO */
          q1->next->coeff.type = rational;
          bigrat_copy(&q1->next->coeff.u.rat, coef.u.rat);
     }
}

void sub_monomial(Polynomial* p, int degree, Coefficient coef)
{
     MonoPtr q, q1;             /* q1 will be one step behind q */
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
     }
     else {
          /* insert between q1 and q */
          q1->next = malloc(sizeof(Monomial));
          q1->next->next = q;
          /* initialise new monomial */
          q1->next->degree = degree;
          /* copy ***RATIONAL ONLY*** TODO */
          q1->next->coeff.type = rational;
          bigrat_copy(&q1->next->coeff.u.rat, coef.u.rat);
          negate_bigrat(&q1->next->coeff.u.rat);
     }
}

void add_polynomials(Polynomial *res, Polynomial left, Polynomial right)
{
     Polynomial old_res;
     MonoPtr p;
     
     if (left.variable != right.variable) {
          printf("Error! Polynomials are in different variables.\n");
          return;
     }

     old_res = *res;

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
     
     if (left.variable != right.variable) {
          printf("Error! Polynomials are in different variables.\n");
          return;
     }

     old_res = *res;

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

