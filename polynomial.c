#include <stdlib.h>
#include <stdio.h>

#include "polynomial.h"
#include "bigrat.h"

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

void free_poly(Polynomial *p)
{
     MonoPtr q, r;
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
          copy_coefficient(&pq->coeff, q->coeff);
     }
     pq->next = p->head;

     free_poly(&old_res);
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
          /* don't print unit coefficient, unless we won't print variable */
          if (!coef_one(m->coeff) || m->degree == 0) {
               print_coefficient(m->coeff);
          }
          /* print variable if degree > 0 */
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
          print_poly(*c.u.poly);
          printf(")");
          break;
     default:
          break;
     }
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
          *c->u.poly = make_zero_poly(s.u.poly->variable);
          copy_poly(c->u.poly, *s.u.poly);
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
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &add_bigrats);
     }
     free_coefficient(&old_res);
}

void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &sub_bigrats);
     }
     free_coefficient(&old_res);
}

void mul_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &mul_bigrats);
     }
     free_coefficient(&old_res);
}

void mul_coefficients2(Coefficient *res, Coefficient left, SHORT_INT_T right)
{
     Coefficient old_res = *res;
     if (left.type == rational) {
          res->type = rational;
          res->u.rat.num = NULL;
          res->u.rat.den = NULL;
          mul_bigrats2(&res->u.rat, left.u.rat, right);
     }
     free_coefficient(&old_res);
}

void div_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &div_bigrats);
     }
     free_coefficient(&old_res);
}

void div_coefficients2(Coefficient *res, Coefficient left, SHORT_INT_T right)
{
     Coefficient old_res = *res;
     if (left.type == rational) {
          res->type = rational;
          res->u.rat.num = NULL;
          res->u.rat.den = NULL;
          div_bigrats2(&res->u.rat, left.u.rat, right);
     }
     free_coefficient(&old_res);
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
     free_coefficient(&old_res);
}

int poly_zero(Polynomial p)
{
     if (p.head->next->coeff.type == special) {
          return 1;
     }
     return 0;
}

int poly_deg(Polynomial p)
{
     return p.head->next->degree;
}

const Coefficient poly_lc(Polynomial p)
{
     return p.head->next->coeff;
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
               q1->next->coeff.u.poly = malloc(sizeof(Polynomial));
               *q1->next->coeff.u.poly = make_zero_poly(coef.u.poly->variable);
               copy_poly(q1->next->coeff.u.poly, *coef.u.poly);
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
          q1->next->degree = degree;
          /* copy ***RATIONAL ONLY*** TODO */
          q1->next->coeff.type = rational;
          init_bigrat(&q1->next->coeff.u.rat);
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

void mul_polynomials(Polynomial *res, Polynomial left, Polynomial right)
{
     Polynomial old_res;
     MonoPtr p, q;
     Coefficient t;
     t.type = special;
     
     if (left.variable != right.variable) {
          printf("Error! Polynomials are in different variables.\n");
          return;
     }

     old_res = *res;

     *res = make_zero_poly(left.variable);

     for (p = left.head->next; p->coeff.type != special; p = p->next) {
          for (q = right.head->next; q->coeff.type != special; q = q->next) {
               mul_coefficients(&t, p->coeff, q->coeff);
               add_monomial(res, p->degree + q->degree, t);
          }
     }

     /* free old result */
     free_poly(&old_res);
}

/* standard polynomial division for coefficients in a field (rationals) */
void div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A, Polynomial B)
{
     Polynomial old_Q, old_R, T, BT;
     int s_degree;
     MonoPtr t;

     if (poly_zero(B)) {
          printf("Error! Polynomial division by zero!\n");
          return;
     }
     if (A.variable != B.variable) {
          printf("Error! Polynomials are in different variables.\n");
          return;
     }

     old_Q = *Q;
     old_R = *R;
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
          div_coefficients(&t->coeff, poly_lc(*R), poly_lc(B));
          add_monomial(Q, s_degree, T.head->next->coeff);
          mul_polynomials(&BT, B, T);
          sub_polynomials(R, *R, BT);
     }

     free_poly(&old_Q);
     free_poly(&T);
     free_poly(&BT);
}

void poly_differentiate(Polynomial *pd, Polynomial p)
{
     MonoPtr q, r;
     
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

void poly_integrate(Polynomial *pi, Polynomial p)
{
     MonoPtr q;

     copy_poly(pi, p);
     for (q = pi->head->next; q->coeff.type != special; q = q->next) {
          /* increase power by one and divide by new power */
          div_coefficients2(&q->coeff, q->coeff, ++q->degree);
     }
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

