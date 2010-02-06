#include <math.h>
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
          copy_coefficient(&pq->coeff, q->coeff);
     }
     pq->next = p->head;

     free_poly(&old_res);
}

void print_poly(Polynomial p)
{
     if (poly_neg(p)) {
          print_poly_sign(p);
     }
     print_poly2(p);
}

void print_poly2(Polynomial p)
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
          if (!coef_one2(m->coeff) || m->degree == 0) {
               print_coefficient2(m->coeff);
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
     } while (m->coeff.type != special && print_coef_sign(m->coeff));
}

void print_poly3(Polynomial p)
{
     print_poly_sign(p);
     print_poly2(p);
}

void print_poly_sign(Polynomial p)
{
     print_coef_sign(poly_lc(p));
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
          print_poly(c.u.poly);
          printf(")");
          break;
     default:
          break;
     }
}

void print_coefficient2(Coefficient c)
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
          print_poly2(c.u.poly);
          printf(")");
          break;
     default:
          break;
     }
}

void print_coefficient3(Coefficient c)
{
     switch (c.type) {
     case rational:
          if (bn_one(c.u.rat.den)) {
               print_bigrat3(c.u.rat);
          }
          else {
               printf("(");
               print_bigrat3(c.u.rat);
               printf(")");
          }
          break;
     case polynomial:
          printf("(");
          print_poly3(c.u.poly);
          printf(")");
          break;
     default:
          break;
     }
}

int print_coef_sign(Coefficient c)
{
     switch (c.type) {
     case rational:
          print_br_sign(c.u.rat);
          break;
     case polynomial:
          print_poly_sign(c.u.poly);
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
     else {
          printf("Warning! Adding coefficients not fully implemented!\n"); /* TODO */
     }
     free_coefficient(&old_res);
}

void sub_coefficients(Coefficient *res, Coefficient left, Coefficient right)
{
     Coefficient old_res = *res;
     if (left.type == rational && right.type == rational) {
          rat_coef_op(res, left, right, &sub_bigrats);
     }
     else {
          printf("Warning! Subbing coefficients not fully implemented!\n"); /* TODO */
     }
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
     else {
          printf("Warning! Muling coefficients not fully implemented!\n"); /* TODO */
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
     else {
          printf("Warning! Diving coefficients not fully implemented!\n"); /* TODO */
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
     else {
          res->type = polynomial;
          res->u.poly.head = NULL;
          poly_power(&res->u.poly, coef.u.poly, power);
     }
     free_coefficient(&old_res);
}

void coef_gcd(Coefficient *res, Coefficient a, Coefficient b)
{
     if (a.type == rational && b.type == rational) {
          res->type = rational;
          res->u.rat.den = make_bignum2(1);
          if (br_int(a.u.rat) && br_int(b.u.rat)) {
               gcd(&res->u.rat.num, a.u.rat.num, b.u.rat.num);
          }
          else {
               /* just return 1 for the gcd of rationals */
               res->u.rat.num = make_bignum2(1);
          }
     }
     else {
          printf("Coefficient GCD not fully implemented.\n");
     }
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
     
     if (left.variable != right.variable) {
          printf("Error! Multivariate polynomials are not supported yet.\n");
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
          printf("Error! Multivariate polynomials are not supported yet.\n");
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
          printf("Error! Multivariate polynomials not supported yet.\n");
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
          printf("Error! Multivariate polynomials are not supported yet.\n");
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

void pseudo_div_polynomials(Polynomial *Q, Polynomial *R, Polynomial A,
                            Polynomial B)
{
     Polynomial old_Q, old_R, T, BT, b;
     int s_degree, N;
     MonoPtr t;

     if (poly_zero(B)) {
          printf("Error! Polynomial division by zero!\n");
          return;
     }
     if (A.variable != B.variable) {
          printf("Error! Multivariate polynomials are not supported yet.\n");
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

     b = make_zero_poly(A.variable);
     add_monomial(&b, 0, poly_lc(B));

     N = poly_deg(A) - poly_deg(B) + 1;
     
     *Q = make_zero_poly(A.variable);
     copy_poly(R, A);
     while (!poly_zero(*R) && (s_degree = poly_deg(*R)-poly_deg(B)) >= 0) {
          t->degree = s_degree;
          copy_coefficient(&t->coeff, poly_lc(*R));
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
     Coefficient coef;
     MonoPtr q;
     
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
          div_coefficients(&q->coeff, q->coeff, coef);
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
          printf("Error! Multivariate polynomials are not supported yet.\n");
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
          if (r->degree == q->degree) {
               /* add coefficients */
               add_coefficients(&q->coeff, q->coeff, r->coeff);
               /* remove zero coefficient */
               if (coef_zero(q->coeff)) {
                    q1->next = q->next;
                    free_coefficient(&q->coeff);
                    free(q);
                    q = NULL;
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
          printf("Error! Multivariate polynomials are not supported yet.\n");
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
          if (r->degree == q->degree) {
               printf("%d,%d\n",r->degree,q->degree);
               /* sub coefficients */
               sub_coefficients(&q->coeff, q->coeff, r->coeff);

               /* remove zero coefficient */
               if (coef_zero(q->coeff)) {
                    q1->next = q->next;
                    free_coefficient(&q->coeff);
                    free(q);
                    q = NULL;
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
     return coef_neg(poly_lc(p));
}

void poly_content(Coefficient *cont, Polynomial p)
{
     Coefficient old_res = *cont;

     MonoPtr q, r;

     if (poly_zero(p)) {
          cont->type = rational;
          cont->u.rat = make_bigrat3(0);
          free_coefficient(&old_res);
          return;
     }

     q = p.head->next;
     r = q->next;
     if (r->coeff.type == special) {
          /* only one coefficient */
          copy_coefficient(cont, q->coeff);
          free_coefficient(&old_res);
          return;
     }

     coef_gcd(cont, q->coeff, r->coeff);

     for (q = r->next; q->coeff.type != special; q = q->next) {
          coef_gcd(cont, *cont, q->coeff);
     }

     free_coefficient(&old_res);
}

void poly_pp(Polynomial *pp, Polynomial p)
{
     Coefficient content = {special};

     poly_content(&content, p);

     if (content.type == rational) {
          div_poly_rat(pp, p, content.u.rat);
     }
     else {
          printf("Polynomial coefficients not supported.\n");
     }

     free_coefficient(&content);
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
